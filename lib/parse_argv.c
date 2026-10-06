// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#include <assert.h>
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "compiler.h"
#include "list.h"
#include "log.h"
#include "parse_argv.h"
#include "patch/strchrnul.h"
#include "strbuf.h"
#include "strlist.h"
#include "strtox.h"
#include "strutil.h"
#include "xalloc.h"

#define __PA_HAS_CMD  (1llu << 63)
#define __PA_SKIP_CMD (1llu << 62)

#define OPT_SHORT (1u << 0)
#define OPT_LONG  (1u << 1)
#define OPT_UNSET (1u << 2)
#define OPT_NO_NO (1u << 4)

#define CLI_LINE_LENGTH   80
#define CLI_OPTION_INDENT 2
#define CLI_OPTION_ALIGN  20
#define CLI_COMMAND_ALIGN 10

enum pa_res {
	PARSE_CONTINUE,
	PARSE_DONE,
	PARSE_DONE_CMD,
};

struct pa_ctx {
	int argc;
	const char **argv;

	int outc;
	const char **outv;

	struct pa_opt *opts;
	const char **usage;

	uint64_t flags;

	struct list_head mode;
};

struct cmdmode {
	struct pa_opt *opt;
	uint64_t flags;

	struct list_head list;
};

struct abbrev {
	struct pa_opt *opt;
	uint64_t flags;
};

static void param_show_help(struct pa_ctx *ctx, FILE *stream);

static struct strbuf path = SB_INIT;

static char *fmt_cli_name(struct pa_opt *opt, uint64_t flags)
{
	struct strbuf sb = SB_INIT;

	if (flags & OPT_SHORT)
		sb_writef(&sb, "flag -%c", opt->alias);
	else
		sb_writef(&sb, "option --%s%s",
			  flags & OPT_UNSET ? "no-" : "", opt->name);

	return sb.buf;
}

static void destroy_cmdmode(struct list_head *mode)
{
	struct cmdmode *c, *tmp;

	list_foreach_entry_safe(c, tmp, mode, list) {
		list_del(&c->list);
		free(c);
	}
}

static void record_cmdmode(struct list_head *mode, struct pa_opt *opt,
			   uint64_t flags)
{
	struct cmdmode *c;

	list_foreach_entry(c, mode, list) {
		if (opt == c->opt || opt->val != c->opt->val)
			continue;

		die("%s and %s cannot be used together",
		    fmt_cli_name(opt, flags), fmt_cli_name(c->opt, c->flags));
	}

	c = xmalloc(sizeof(*c));
	c->opt = opt;
	c->flags = flags;

	list_add(&c->list, mode);
}

static void mark_abbrev(struct pa_opt *opt, uint64_t flags,
			struct abbrev *abbrev, struct abbrev *ambigu)
{
	if (abbrev->opt && abbrev->opt != opt) {
		ambigu->opt = abbrev->opt;
		ambigu->flags = abbrev->flags;
	}

	abbrev->opt = opt;
	abbrev->flags = flags;
}

static int has_command(struct pa_opt *opts)
{
	struct pa_opt *opt;

	__foreach_opt(opt, opts) {
		if (opt->class == PA_OPTC_COMMAND)
			return 1;
	}

	return 0;
}

static const char *default_command(struct pa_opt *opts)
{
	struct pa_opt *opt;

	__foreach_opt(opt, opts) {
		if (opt->class != PA_OPTC_COMMAND)
			continue;

		if (*opt->cmd_val == opt->cmd_fn)
			return opt->name;
	}

	cc_trap();
}

static void die_no_bare(const char *s)
{
	die("'%s' takes no bare arguments, but got '%s'", path.buf, s);
}

static enum pa_res parse_command(struct pa_ctx *ctx, const char *name)
{
	struct pa_opt *opt;

	__foreach_opt(opt, ctx->opts) {
		if (opt->class != PA_OPTC_COMMAND || strcmp(opt->name, name))
			continue;

		*opt->cmd_val = opt->cmd_fn;
		return PARSE_DONE_CMD;
	}

	if (ctx->flags & PA_LAX_CMD) {
		if (ctx->flags & PA_NO_BARE)
			die_no_bare(name);

		ctx->outc++;
		ctx->flags |= __PA_SKIP_CMD;
		return PARSE_CONTINUE;
	}

	die("unknown command '%s', see '%s -h'", name, path.buf);
}

static void assign_flag(struct pa_opt *opt, const char *arg, uint64_t flags)
{
	uint64_t *p = opt->val;

	if (flags & OPT_UNSET)
		*p &= ~opt->ext;
	else
		*p |= opt->ext;
}

static void assign_count(struct pa_opt *opt, const char *arg, uint64_t flags)
{
	int *p = opt->val;

	if (flags & OPT_UNSET)
		*p = 0;
	else
		*p += 1;
}

#define bug_abuse(opt, flags, abuse) \
	bug("%s abuses %s", fmt_cli_name(opt, flags), abuse);

static void __assign_number(struct pa_opt *opt, const char *str, uint64_t flags)
{
	int err;

	if (opt->flags & PA_OPT_SIGNED) {
		if (opt->flags & PA_OPT_LLONG)
			err = strxtoll(str, 0, (long long *)opt->val);
		else
			err = strxtos(str, 0, (int *)opt->val);
	} else {
		if (opt->flags & PA_OPT_LLONG)
			err = strxtoull(str, 0, (unsigned long long *)opt->val);
		else
			err = strxtou(str, 0, (unsigned int *)opt->val);
	}

	if (err)
		die_errno("failed to parse integer '%s' for %s", str,
			  fmt_cli_name(opt, flags));
}

static void assign_number(struct pa_opt *opt, const char *arg, uint64_t flags)
{
	if (flags & OPT_UNSET)
		memset(opt->val, 0, opt->flags & PA_OPT_LLONG ?
				    sizeof(long long) : sizeof(int));
	else if (arg)
		__assign_number(opt, arg, flags);
	else if (opt->flags & PA_OPT_LAX_ARG)
		__assign_number(opt, (const char *)opt->ext, flags);
	else
		bug_abuse(opt, flags, "PA_OPT_NO_ARG");
}

static void assign_string(struct pa_opt *opt, const char *arg, uint64_t flags)
{
	const char **p = opt->val;

	if (flags & OPT_UNSET)
		*p = NULL;
	else if (arg)
		*p = arg;
	else if (opt->flags & PA_OPT_LAX_ARG)
		*p = (const char *)opt->ext;
	else
		bug_abuse(opt, flags, "PA_OPT_NO_ARG");
}

static void assign_cmdmode(struct pa_opt *opt, const char *arg, uint64_t flags)
{
	*(int *)opt->val = opt->ext;
}

static void assign_pick(struct pa_opt *opt, const char *arg, uint64_t flags)
{
	const char **p = opt->val;
	const char **options = (const char **)opt->ext;
	struct strbuf sb = SB_INIT;

	if (flags & OPT_UNSET)
		*p = NULL;
	else if (arg)
		*p = arg;
	else if (opt->flags & PA_OPT_LAX_ARG)
		*p = *options;
	else
		bug_abuse(opt, flags, "PA_OPT_NO_ARG");

	if (!*p)
		return;

	while (*options) {
		if (!strcmp(*p, *options++))
			return;
	}

	options = (const char **)opt->ext;

	while (*options)
		sb_writef(&sb, "  %s\n", *options++);

	sb_write_ch_at(&sb, sb.len - 1, 0);

	die("invalid pick on '%s' for %s, valid options are:\n%s", *p,
	    fmt_cli_name(opt, flags), sb.buf);
}

static void assign_value(struct pa_ctx *ctx, struct pa_opt *opt,
			 const char *arg, uint64_t flags)
{
	static typeof(assign_number) *map[PA_OPTT__COUNT] = {
		[PA_OPTT_FLAG]    = assign_flag,
		[PA_OPTT_COUNT]   = assign_count,
		[PA_OPTT_NUMBER]  = assign_number,
		[PA_OPTT_STRING]  = assign_string,
		[PA_OPTT_CMDMODE] = assign_cmdmode,
		[PA_OPTT_PICK]    = assign_pick,
	};

	map[opt->type](opt, arg, flags);

	if (opt->type == PA_OPTT_CMDMODE)
		record_cmdmode(&ctx->mode, opt, flags);
}

static const char *fmt_flag(const char *s)
{
	static struct strlist ref = SL_INIT(ref, SL_USE_REF);

	size_t len = strlen(s);
	char *buf = xmalloc(1 + len + 1);

	memcpy(&buf[1], s, len + 1);
	buf[0] = '-';

	sl_push(&ref, buf);
	return buf;
}

static enum pa_res parse_flag(struct pa_ctx *ctx, const char **__flag)
{
	const char *flag = *__flag;
	struct pa_opt *opt;

	__foreach_opt(opt, ctx->opts) {
		const char *arg = NULL;
		const char *ret = &flag[1];

		if (opt->class != PA_OPTC_OPTION || opt->alias != flag[0])
			continue;

		if (!(opt->flags & PA_OPT_NO_ARG)) {
			if (flag[1]) {
				arg = &flag[1];
				ret = "";
			} else if (ctx->argc > 1) {
				arg = ctx->argv[1];
				ctx->argc--;
				ctx->argv++;
			} else if (!(opt->flags & PA_OPT_LAX_ARG)) {
				die("flag -%c requires a value", opt->alias);
			}
		}

		assign_value(ctx, opt, arg, 0);
		*__flag = ret;
		return PARSE_CONTINUE;
	}

	if (ctx->flags & PA_STOP_UNKNOWN) {
		ctx->argv[0] = fmt_flag(flag);
		return PARSE_DONE;
	}

	die("unknown flag -%c", flag[0]);
}

static int __parse_option(struct pa_ctx *ctx, struct pa_opt *opt,
			  const char *arg, uint64_t flags)
{
	if (arg[0] == '=') {
		if (opt->flags & PA_OPT_NO_ARG || flags & OPT_UNSET)
			die("%s takes no value", fmt_cli_name(opt, flags));
		arg += 1;
	} else if (arg[0]) {
		return -1;
	} else if (opt->flags & PA_OPT_NO_ARG || flags & OPT_UNSET) {
		arg = NULL;
	} else if (ctx->argc > 1) {
		arg = ctx->argv[1];
		ctx->argc--;
		ctx->argv++;
	} else if (!(opt->flags & PA_OPT_LAX_ARG)) {
		die("%s requires a value", fmt_cli_name(opt, flags));
	} else {
		arg = NULL;
	}

	assign_value(ctx, opt, arg, flags);
	return 0;
}

static enum pa_res parse_option(struct pa_ctx *ctx, const char *option)
{
	struct pa_opt *opt;
	const char *cli_name = option;
	const char *sep;
	uint64_t cli_flags = 0;
	const char *arg = NULL;

	struct abbrev abbrev = { 0 };
	struct abbrev ambigu = { 0 };

	if (!str_skip2(cli_name, "no-", &cli_name)) {
		if (!str_skip2(cli_name, "no-", &cli_name))
			cli_flags |= OPT_NO_NO;
		else
			cli_flags |= OPT_UNSET;
	}

	sep = strchrnul(cli_name, '=');

	if (sep == cli_name)
		die("option --%s is invalid", sep);

	__foreach_opt(opt, ctx->opts) {
		const char *opt_name = opt->name;
		uint64_t opt_flags = 0;
		uint64_t flags;

		if (opt->class != PA_OPTC_OPTION || !opt->name)
			continue;

		if (!str_skip2(opt_name, "no-", &opt_name))
			opt_flags |= OPT_UNSET;
		else if (cli_flags & OPT_NO_NO)
			continue;

		flags = (cli_flags ^ opt_flags) & OPT_UNSET;

		if (flags & OPT_UNSET && opt->flags & PA_OPT_NO_NEG)
			continue;

		if (!str_skip2(cli_name, opt_name, &arg)) {
			if (__parse_option(ctx, opt, arg, flags))
				continue;
			return PARSE_CONTINUE;
		}

		if (!strncmp(opt_name, cli_name, sep - cli_name))
			mark_abbrev(opt, flags, &abbrev, &ambigu);

		if (!(opt->flags & PA_OPT_NO_NEG) && str_skip("no-", option))
			mark_abbrev(opt, opt_flags ^ OPT_UNSET, &abbrev,
				    &ambigu);
	}

	if (ambigu.opt) {
		die("ambiguous option --%s, could be %s or %s", option,
		    fmt_cli_name(ambigu.opt, ambigu.flags),
		    fmt_cli_name(abbrev.opt, abbrev.flags));
	} else if (abbrev.opt) {
		if (*sep)
			arg = sep;
		else
			arg = "";

		__parse_option(ctx, abbrev.opt, arg, abbrev.flags);
	} else {
		if (ctx->flags & PA_STOP_UNKNOWN)
			return PARSE_DONE;

		die("unknown option %s", option);
	}

	return PARSE_CONTINUE;
}

static enum pa_res parse_cli_arg(struct pa_ctx *ctx)
{
	enum pa_res res = PARSE_CONTINUE;
	const char *str = ctx->argv[0];

	if (str[0] != '-') {
		if (ctx->flags & PA_STOP_BARE)
			return PARSE_DONE;
		else if (ctx->flags & __PA_HAS_CMD &&
			 !(ctx->flags & __PA_SKIP_CMD))
			res = parse_command(ctx, str);
		else if (ctx->flags & PA_NO_BARE)
			die_no_bare(str);

		if (res)
			return res;

		ctx->outv[ctx->outc] = str;
		ctx->outc += 1;
	} else if (str[1] != '-') {
		str += 1;

		if (str[0] == 'h' && str[1] == 0)
			param_show_help(ctx, stdout);

		if (!str[0]) {
			if (ctx->flags & PA_STOP_UNKNOWN)
				return PARSE_DONE;
			die("unknown flag '-'");
		}

		while (*str && !(res = parse_flag(ctx, &str)));
	} else {
		str += 2;

		if (str[0] == 0) {
			if (ctx->flags & __PA_HAS_CMD &&
			    !(ctx->flags & PA_LAX_CMD)) {
				error("'%s' requires a command", path.buf);
				param_show_help(ctx, stderr);
			}

			ctx->argc--;
			ctx->argv++;
			return PARSE_DONE;
		}

		if (!strcmp(str, "help"))
			param_show_help(ctx, stdout);

		res = parse_option(ctx, str);
	}

	return res;
}

int pa_parse_args(int argc, const char **argv, struct pa_opt *opts,
		  const char **usage, uint64_t flags)
{
	int ret;
	struct pa_ctx ctx = {
		.argc  = argc - 1,
		.argv  = argv + 1,

		.outc  = 0,
		.outv  = argv,

		.opts  = opts,
		.usage = usage,

		.flags = flags,

		.mode  = LIST_HEAD_INIT(ctx.mode),
	};
	int has_cmd = 0;

	if (has_command(opts))
		ctx.flags |= __PA_HAS_CMD;

	if (flags & PA_LAX_CMD)
		ctx.flags |= PA_STOP_UNKNOWN;

	assert(argc >= 1);

	if (ctx.flags & __PA_HAS_CMD)
		assert(!(ctx.flags & PA_STOP_BARE));

	if (path.len)
		sb_write_ch(&path, ' ');
	sb_write_str(&path, argv[0]);

	while (ctx.argc) {
		enum pa_res res = parse_cli_arg(&ctx);

		switch (res) {
		case PARSE_DONE_CMD:
			has_cmd = 1;
		case PARSE_DONE:
			goto out;
		}

		ctx.argc--;
		ctx.argv++;
	}

out:
	if (!list_is_empty(&ctx.mode))
		destroy_cmdmode(&ctx.mode);

	if (ctx.flags & PA_LAX_CMD && !has_cmd) {
		ctx.outv[0] = default_command(opts);

		if (!(ctx.flags & __PA_SKIP_CMD))
			ctx.outc += 1;
	}

	ret = ctx.outc + ctx.argc;

	if (ctx.flags & __PA_HAS_CMD && !(ctx.flags & PA_LAX_CMD) && !ret) {
		if (argc > 1)
			error("'%s' requires a command", path.buf);

		param_show_help(&ctx, argc > 1 ? stderr : stdout);
	}

	if (ctx.argc)
		memmove(&ctx.outv[ctx.outc], ctx.argv,
			ctx.argc * sizeof(*ctx.argv));

	ctx.outv[ret] = NULL;
	return ret;
}

static void show_cmd_usage(struct strlist *sl, FILE *stream, const char **usage)
{
	const char *prefix = "usage: ";
	size_t prefix_len = strlen("usage: ");

	while (*usage) {
		const char *line = *usage;
		const char *rest = line;
		size_t cmd_len;
		size_t pad;
		size_t wrap;
		char *str;

		while (isalpha(*rest) || isspace(*rest))
			rest++;

		cmd_len = rest - line;
		pad = prefix_len + cmd_len;
		wrap = CLI_LINE_LENGTH - prefix_len;

		if ((CLI_LINE_LENGTH >> 1) > pad)
			wrap -= cmd_len;

		fprintf(stream, "%*s%.*s", (int)prefix_len, prefix,
			(int)cmd_len, line);

		if (!*rest) {
			putc('\n', stream);
			goto next;
		}

		sl_read_line(sl, rest, wrap);

		str = sl_pop(sl);
		fputs(str, stream);
		putc('\n', stream);

		while (str = sl_pop(sl))
			fprintf(stream, "%*s%s\n", (int)pad, "", str);

next:
		usage++;
		prefix = "or: ";
	}

	putc('\n', stream);
}

static void show_opt_usage(struct strlist *sl, FILE *stream,
			   struct pa_opt *opts)
{
	unsigned int cnt = 0;
	struct pa_opt *opt;

	__foreach_opt(opt, opts) {
		const char *fmt = opt->flags & PA_OPT_NO_NEG ? "--%s" :
							       "--[no-]%s";
		size_t len = CLI_OPTION_INDENT;
		size_t pad = CLI_OPTION_ALIGN + 2;
		char *str;

		switch (opt->class) {
		case PA_OPTC_GROUP:
			if (opt != opts)
				putc('\n', stream);

			fputs(opt->name, stream);
			putc('\n', stream);
			continue;
		case PA_OPTC_COMMAND:
			fmt = "%s";
			pad = CLI_COMMAND_ALIGN + 2;
		}

		fprintf(stream, "%*s", (int)len, "");

		if (opt->alias)
			len += fprintf(stream, "-%c, ", opt->alias);

		len += fprintf(stream, fmt, opt->name);

		if (opt->vhelp) {
			const char *has_group = strpbrk(opt->vhelp, "()<>[]|");

			if (opt->flags & PA_OPT_LAX_ARG)
				fmt = has_group ? "[=%s]" : "[=<%s>]";
			else
				fmt = has_group ? " %s" : " <%s>";

			len += fprintf(stream, fmt, opt->vhelp);
		}

		if (len >= CLI_OPTION_ALIGN) {
			putc('\n', stream);
			len = 0;
		}

		sl_read_line(sl, opt->help, CLI_LINE_LENGTH - pad);

		str = sl_pop(sl);
		fprintf(stream, "%*s%s\n", (int)(pad - len), "", str);

		while (str = sl_pop(sl))
			fprintf(stream, "%*s%s\n", (int)pad, "", str);

		cnt++;
	}

	if (cnt)
		putc('\n', stream);
}

static void param_show_help(struct pa_ctx *ctx, FILE *stream)
{
	struct strlist sl;

	sl_init(&sl, SL_USE_SB);
	show_cmd_usage(&sl, stream, ctx->usage);
	show_opt_usage(&sl, stream, ctx->opts);

	exit(stream == stderr ? 1 : 0);
}
