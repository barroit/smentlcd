/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#ifndef PARSE_ARGS_H
#define PARSE_ARGS_H

#include <stddef.h>
#include <stdint.h>

#define PA_STOP_BARE    (1u << 0) /* stop parsing on a bare argument */
#define PA_LAX_CMD      (1u << 1) /* accept an optional command */
#define PA_NO_BARE      (1u << 2) /* abort on a bare argument */
#define PA_STOP_UNKNOWN (1u << 3) /* stop parsing on an argument */

#define PA_OPT_NO_ARG  (1u << 0) /* don't accept a value */
#define PA_OPT_LAX_ARG (1u << 1) /* accept an optional value */
#define PA_OPT_SIGNED  (1u << 3) /* treat opt->val value as signed */
#define PA_OPT_LLONG   (1u << 4) /* treat opt->val type as long long */
#define PA_OPT_NO_NEG  (1u << 5) /* disable '--no-' variant */

typedef int (*pa_command_fn)(int, const char **);

enum pa_opt_class {
	PA_OPTC_END,
	PA_OPTC_OPTION,
	PA_OPTC_COMMAND,
	PA_OPTC_GROUP,
};

enum pa_opt_type {
	PA_OPTT_FLAG,
	PA_OPTT_COUNT,
	PA_OPTT_NUMBER,
	PA_OPTT_STRING,
	PA_OPTT_CMDMODE,
	PA_OPTT_PICK,

	PA_OPTT__COUNT,
};

struct pa_opt {
	enum pa_opt_class class;
	const char *name;
	char alias;

	enum pa_opt_type type;
	union {
		void *val;
		pa_command_fn *cmd_val;
	};
	union {
		intptr_t ext;
		pa_command_fn cmd_fn;
	};

	uint64_t flags;

	const char *help;
	const char *vhelp;
};

#define PA_OPT_END() { .class = PA_OPTC_END }

#define PA_OPT_GROUP(n) { .class = PA_OPTC_GROUP, .name = n }

#define PA_OPT_CMD(n, v, f, h) {	\
	.class   = PA_OPTC_COMMAND,	\
	.name    = n,			\
	.cmd_val = v,			\
	.cmd_fn  = f,			\
	.help    = h,			\
}

#define PA_OPT_FLAG(n, a, v, e, h) PA_OPT_FLAG_F(n, a, v, e, 0, h)
#define PA_OPT_FLAG_F(n, a, v, e, f, h) \
	__PA_OPT(n, a, PA_OPTT_FLAG, v, e, PA_OPT_NO_ARG | (f), NULL, h)

#define PA_OPT_COUNT(n, a, v, h) PA_OPT_COUNT_F(n, a, v, 0, h)
#define PA_OPT_COUNT_F(n, a, v, f, h) \
	__PA_OPT(n, a, PA_OPTT_COUNT, v, 0, PA_OPT_NO_ARG | (f), NULL, h)

#define PA_OPT_FILE(n, a, v, h) PA_OPT_STRING_P(n, a, v, "path", h)
#define PA_OPT_STRING(n, a, v, h) PA_OPT_STRING_F(n, a, v, 0, h)
#define PA_OPT_STRING_F(n, a, v, f, h) PA_OPT_STRING_FP(n, a, v, f, NULL, h)
#define PA_OPT_STRING_P(n, a, v, p, h) PA_OPT_STRING_FP(n, a, v, 0, p, h)
#define PA_OPT_STRING_FP(n, a, v, f, p, h) \
	__PA_OPT(n, a, PA_OPTT_STRING, v, NULL, f, p, h)

#define PA_OPT_NUMBER(n, a, v, h) PA_OPT_NUMBER_F(n, a, v, 0, h)
#define PA_OPT_NUMBER_F(n, a, v, f, h) PA_OPT_NUMBER_EF(n, a, v, NULL, f, h)
#define PA_OPT_NUMBER_EF(n, a, v, e, f, h) \
	__PA_OPT(n, a, PA_OPTT_NUMBER, v, e, f, NULL, h)

#define PA_OPT_CMDMODE(n, a, v, e, h) \
	__PA_OPT(n, a, PA_OPTT_CMDMODE, v, e, PA_OPT_NO_ARG | PA_OPT_NO_NEG, \
		 NULL, h)

#define PA_OPT_PICK(n, a, v, e, h) PA_OPT_PICK_F(n, a, v, e, 0, h)
#define PA_OPT_PICK_F(n, a, v, e, f, h) \
	__PA_OPT(n, a, PA_OPTT_PICK, v, e, f, "mode", h)

#define __PA_OPT(n, a, t, v, e, f, p, h) {	\
	.class = PA_OPTC_OPTION,		\
	.name  = n,				\
	.alias = a,				\
	.type  = t,				\
	.val   = v,				\
	.ext   = (intptr_t)e,			\
	.flags = f,				\
	.help  = h,				\
	.vhelp = p,				\
}

#define __foreach_opt(ent, start) \
	for (ent = start; (ent)->class != PA_OPTC_END; (ent)++)

int pa_parse_args(int argc, const char **argv,
		  struct pa_opt *opts, const char **usage, uint64_t flags);

#endif /* PARSE_ARGS_H */
