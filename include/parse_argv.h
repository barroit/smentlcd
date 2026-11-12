/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#ifndef PARSE_ARGS_H
#define PARSE_ARGS_H

#include <stddef.h>
#include <stdint.h>

/**
 * macro PA_STOP_BARE - Stop parsing at the first bare argument.
 *
 * Do not use with command entries.
 */
#define PA_STOP_BARE (1u << 0)

/**
 * macro PA_LAX_CMD - Allow optional command selection.
 *
 * Initialize the command target to the default command handler before parsing.
 * If no command is selected, the parser inserts the default command into argv.
 *
 * Enabling this flag also enables PA_STOP_UNKNOWN.
 */
#define PA_LAX_CMD (1u << 1)

/**
 * macro PA_NO_BARE - Reject bare arguments.
 */
#define PA_NO_BARE (1u << 2)

/**
 * macro PA_STOP_UNKNOWN - Stop parsing at an unknown option or flag.
 */
#define PA_STOP_UNKNOWN (1u << 3)

/**
 * macro PA_OPT_NO_ARG - Disallow an option value.
 */
#define PA_OPT_NO_ARG (1u << 0)

/**
 * macro PA_OPT_LAX_ARG - Allow an optional value.
 */
#define PA_OPT_LAX_ARG (1u << 1)

/**
 * macro PA_OPT_SIGNED - Treat a numeric target as signed.
 */
#define PA_OPT_SIGNED (1u << 3)

/**
 * macro PA_OPT_LLONG - Use a long long integer target.
 */
#define PA_OPT_LLONG (1u << 4)

/**
 * macro PA_OPT_NO_NEG - Disable option negation.
 */
#define PA_OPT_NO_NEG (1u << 5)

/**
 * typedef pa_command_fn - Command handler function pointer.
 * @argc: Number of arguments.
 * @argv: Argument vector.
 *
 * Return: Command handler result.
 */
typedef int (*pa_command_fn)(int argc, const char **argv);

/**
 * enum pa_opt_class - Parser entry classes.
 * @PA_OPTC_END: End of an entry array.
 * @PA_OPTC_OPTION: Option or flag entry.
 * @PA_OPTC_COMMAND: Command entry.
 * @PA_OPTC_GROUP: Group entry.
 */
enum pa_opt_class {
	PA_OPTC_END,
	PA_OPTC_OPTION,
	PA_OPTC_COMMAND,
	PA_OPTC_GROUP,
};

/**
 * enum pa_opt_type - Parser entry types.
 * @PA_OPTT_FLAG: Flag entry.
 * @PA_OPTT_COUNT: Counter entry.
 * @PA_OPTT_NUMBER: Numeric entry.
 * @PA_OPTT_STRING: String entry.
 * @PA_OPTT_CMDMODE: Command mode entry.
 * @PA_OPTT_PICK: Validated string entry.
 * @PA_OPTT__COUNT: Number of entry types.
 */
enum pa_opt_type {
	PA_OPTT_FLAG,
	PA_OPTT_COUNT,
	PA_OPTT_NUMBER,
	PA_OPTT_STRING,
	PA_OPTT_CMDMODE,
	PA_OPTT_PICK,

	PA_OPTT__COUNT,
};

/**
 * struct pa_opt - Parser entry definition.
 * @class: Entry class.
 * @name: Option or command name.
 * @alias: Option alias.
 * @type: Entry type.
 * @val: Pointer to the target object.
 * @cmd_val: Pointer to the selected command handler.
 * @ext: Entry-specific value.
 * @cmd_fn: Command handler.
 * @flags: Entry behavior flags.
 * @help: Help message.
 * @vhelp: Value hint.
 */
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

/**
 * macro PA_OPT_END - Terminate an option array.
 */
#define PA_OPT_END() { .class = PA_OPTC_END }

/**
 * macro PA_OPT_GROUP - Add an option group.
 * @n: Group name.
 */
#define PA_OPT_GROUP(n) { .class = PA_OPTC_GROUP, .name = n }

/**
 * macro PA_OPT_CMD - Add a command entry.
 * @n: Command name.
 * @v: Pointer to the selected command handler.
 * @f: Command handler.
 * @h: Help message.
 */
#define PA_OPT_CMD(n, v, f, h) {	\
	.class   = PA_OPTC_COMMAND,	\
	.name    = n,			\
	.cmd_val = v,			\
	.cmd_fn  = f,			\
	.help    = h,			\
}

/**
 * macro PA_OPT_FLAG - Add a flag entry.
 * @n: Option name.
 * @a: Option alias.
 * @v: Pointer to a ``uint64_t`` target.
 * @e: Bit mask.
 * @h: Help message.
 */
#define PA_OPT_FLAG(n, a, v, e, h) PA_OPT_FLAG_F(n, a, v, e, 0, h)

/**
 * macro PA_OPT_FLAG_F - Add a flag entry with custom flags.
 * @n: Option name.
 * @a: Option alias.
 * @v: Pointer to a ``uint64_t`` target.
 * @e: Bit mask.
 * @f: Entry behavior flags.
 * @h: Help message.
 *
 * Set the bit(s) specified by @e when the option is set, or clear them when the
 * option is unset.
 */
#define PA_OPT_FLAG_F(n, a, v, e, f, h) \
	__PA_OPT(n, a, PA_OPTT_FLAG, v, e, PA_OPT_NO_ARG | (f), NULL, h)

/**
 * macro PA_OPT_COUNT - Add a counter entry.
 * @n: Option name.
 * @a: Option alias.
 * @v: Pointer to an ``int`` target.
 * @h: Help message.
 */
#define PA_OPT_COUNT(n, a, v, h) PA_OPT_COUNT_F(n, a, v, 0, h)

/**
 * macro PA_OPT_COUNT_F - Add a counter entry with custom flags.
 * @n: Option name.
 * @a: Option alias.
 * @v: Pointer to an ``int`` target.
 * @f: Entry behavior flags.
 * @h: Help message.
 *
 * Increment the target when the option is set, or reset it to zero when the
 * option is unset.
 */
#define PA_OPT_COUNT_F(n, a, v, f, h) \
	__PA_OPT(n, a, PA_OPTT_COUNT, v, 0, PA_OPT_NO_ARG | (f), NULL, h)

/**
 * macro PA_OPT_FILE - Add a string entry with a ``path`` hint.
 * @n: Option name.
 * @a: Option alias.
 * @v: Pointer to a ``const char *`` target.
 * @h: Help message.
 */
#define PA_OPT_FILE(n, a, v, h) PA_OPT_STRING_P(n, a, v, "path", h)

/**
 * macro PA_OPT_STRING - Add a string entry.
 * @n: Option name.
 * @a: Option alias.
 * @v: Pointer to a ``const char *`` target.
 * @h: Help message.
 */
#define PA_OPT_STRING(n, a, v, h) PA_OPT_STRING_F(n, a, v, 0, h)

/**
 * macro PA_OPT_STRING_F - Add a string entry with custom flags.
 * @n: Option name.
 * @a: Option alias.
 * @v: Pointer to a ``const char *`` target.
 * @f: Entry behavior flags.
 * @h: Help message.
 */
#define PA_OPT_STRING_F(n, a, v, f, h) PA_OPT_STRING_FP(n, a, v, f, NULL, h)

/**
 * macro PA_OPT_STRING_P - Add a string entry with a custom hint.
 * @n: Option name.
 * @a: Option alias.
 * @v: Pointer to a ``const char *`` target.
 * @p: Value hint.
 * @h: Help message.
 */
#define PA_OPT_STRING_P(n, a, v, p, h) PA_OPT_STRING_FP(n, a, v, 0, p, h)

/**
 * macro PA_OPT_STRING_FP - Add a string entry with custom flags and a hint.
 * @n: Option name.
 * @a: Option alias.
 * @v: Pointer to a ``const char *`` target.
 * @f: Entry behavior flags.
 * @p: Value hint.
 * @h: Help message.
 *
 * Assign the supplied string to the target when the option is set, or NULL when
 * the option is unset.
 *
 * With PA_OPT_LAX_ARG, assign NULL when no value is supplied.
 */
#define PA_OPT_STRING_FP(n, a, v, f, p, h) \
	__PA_OPT(n, a, PA_OPTT_STRING, v, NULL, f, p, h)

/**
 * macro PA_OPT_NUMBER - Add a numeric entry.
 * @n: Option name.
 * @a: Option alias.
 * @v: Pointer to the numeric target.
 * @h: Help message.
 */
#define PA_OPT_NUMBER(n, a, v, h) PA_OPT_NUMBER_F(n, a, v, 0, h)

/**
 * macro PA_OPT_NUMBER_F - Add a numeric entry with custom flags.
 * @n: Option name.
 * @a: Option alias.
 * @v: Pointer to the numeric target.
 * @f: Entry behavior flags.
 * @h: Help message.
 */
#define PA_OPT_NUMBER_F(n, a, v, f, h) PA_OPT_NUMBER_EF(n, a, v, NULL, f, h)

/**
 * macro PA_OPT_NUMBER_EF - Add a numeric entry with a default and custom flags.
 * @n: Option name.
 * @a: Option alias.
 * @v: Pointer to the numeric target.
 * @e: Default numeric string.
 * @f: Entry behavior flags.
 * @h: Help message.
 *
 * Use an ``unsigned int`` target by default. Use PA_OPT_SIGNED for an ``int``
 * target, PA_OPT_LLONG for an ``unsigned long long`` target, or both for a
 * ``long long`` target.
 *
 * With PA_OPT_LAX_ARG, parse @e when no value is supplied. Reset the target to
 * zero when the option is unset.
 */
#define PA_OPT_NUMBER_EF(n, a, v, e, f, h) \
	__PA_OPT(n, a, PA_OPTT_NUMBER, v, e, f, NULL, h)

/**
 * macro PA_OPT_CMDMODE - Add a command mode entry.
 * @n: Option name.
 * @a: Option alias.
 * @v: Pointer to an ``int`` target.
 * @e: Mode value.
 * @h: Help message.
 *
 * Set the target to @e. Command mode entries sharing @v are mutually exclusive.
 */
#define PA_OPT_CMDMODE(n, a, v, e, h) \
	__PA_OPT(n, a, PA_OPTT_CMDMODE, v, e, PA_OPT_NO_ARG | PA_OPT_NO_NEG, \
		 NULL, h)

/**
 * macro PA_OPT_PICK - Add a validated string entry.
 * @n: Option name.
 * @a: Option alias.
 * @v: Pointer to a ``const char *`` target.
 * @e: NULL-terminated array of accepted strings.
 * @h: Help message.
 */
#define PA_OPT_PICK(n, a, v, e, h) PA_OPT_PICK_F(n, a, v, e, 0, h)

/**
 * macro PA_OPT_PICK_F - Add a validated string entry with custom flags.
 * @n: Option name.
 * @a: Option alias.
 * @v: Pointer to a ``const char *`` target.
 * @e: NULL-terminated array of accepted strings.
 * @f: Entry behavior flags.
 * @h: Help message.
 *
 * Require the supplied string to match an item in @e.
 *
 * With PA_OPT_LAX_ARG, select the first item in @e when no value is supplied.
 * Set the target to NULL when the option is unset.
 */
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

/**
 * pa_parse_args() - Parse command line arguments in-place.
 * @argc: Number of input arguments.
 * @argv: Argument vector used for input and output.
 * @opts: Option array terminated by PA_OPT_END().
 * @usage: NULL-terminated array of usage lines.
 * @flags: Parser behavior flags.
 *
 * Write remaining arguments to @argv starting at index zero and append a NULL
 * terminator.
 *
 * Return: Number of remaining arguments.
 */
int pa_parse_args(int argc, const char **argv,
		  struct pa_opt *opts, const char **usage, uint64_t flags);

#endif /* PARSE_ARGS_H */
