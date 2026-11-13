/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2024, 2025 Jiamu Sun <barroit@linux.com>
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#ifndef STRLIST_H
#define STRLIST_H

#include <stdint.h>
#include <stddef.h>

#include "list.h"

struct strbuf;

struct strlist_item {
	union {
		struct strbuf *sb;
		const char *ref;
		char *cp;
	};

	struct list_head list;
};

/**
 * struct strlist - List of strings with configurable storage.
 * @head: List of active entries.
 * @idle: List of reusable entries.
 * @items: Number of active entries.
 * @flags: Storage mode and behavior flags.
 */
struct strlist {
	struct list_head head;
	struct list_head idle;

	size_t items;

	uint64_t flags;
};

/**
 * macro SL_USE_CP - Store independent copies of strings.
 */
#define SL_USE_CP (1u << 0)

/**
 * macro SL_USE_REF - Store references to strings without copying them.
 */
#define SL_USE_REF (1u << 1)

/**
 * macro SL_USE_SB - Store strings in reusable strbuf buffers.
 */
#define SL_USE_SB (1u << 2)

/**
 * macro SL_RET_LEN - Return string lengths from push operations.
 */
#define SL_RET_LEN (1u << 3)

/**
 * macro SL_POP_DUP - Duplicate strings returned by sl_pop().
 */
#define SL_POP_DUP (1u << 4)

#define __sl_mode_mask (SL_USE_CP | SL_USE_REF | SL_USE_SB)
#define __sl_mode(f)   ((f) & __sl_mode_mask)

/**
 * macro SL_INIT_CP - Initialize a list with copied strings and length returns.
 * @s: List variable name.
 */
#define SL_INIT_CP(s) SL_INIT(s, SL_USE_CP | SL_RET_LEN)

/**
 * macro SL_INIT - Initialize a list with specified flags.
 * @s: List variable name.
 * @f: Storage mode and behavior flags.
 *
 * Specify exactly one storage mode.
 */
#define SL_INIT(s, f) {				\
	.head  = LIST_HEAD_INIT(s.head),	\
	.idle  = LIST_HEAD_INIT(s.idle),	\
	.flags = f,				\
}

/**
 * sl_init() - Initialize a string list.
 * @sl: List to initialize.
 * @flags: Storage mode and behavior flags.
 *
 * Default to SL_USE_CP if no storage mode is specified. Specify at most one
 * storage mode.
 */
void sl_init(struct strlist *sl, uint64_t flags);

/**
 * sl_destroy() - Release list storage.
 * @sl: List to destroy.
 *
 * Free remaining strings and cached entries according to the storage mode.
 */
void sl_destroy(struct strlist *sl);

/**
 * sl_push() - Insert a string at the front of a list.
 * @sl: Destination list.
 * @str: NUL-terminated string to insert.
 *
 * Return: Length of @str if SL_RET_LEN is set, or ``(size_t)-1`` otherwise.
 */
size_t sl_push(struct strlist *sl, const char *str);

/**
 * sl_push_back() - Insert a string at the back of a list.
 * @sl: Destination list.
 * @str: NUL-terminated string to insert.
 *
 * Return: Length of @str if SL_RET_LEN is set, or ``(size_t)-1`` otherwise.
 */
size_t sl_push_back(struct strlist *sl, const char *str);

/**
 * sl_pop() - Remove and return the first string in a list.
 * @sl: List to pop from.
 *
 * With SL_USE_CP, the caller must free the returned string.
 * With SL_USE_REF, the returned pointer references the original string.
 * With SL_USE_SB, subsequent push operations may invalidate the pointer.
 *
 * SL_POP_DUP returns an allocated copy in reference and buffer modes. The
 * caller must free the copy.
 *
 * Return: Pointer to the string, or NULL if the list is empty.
 */
char *sl_pop(struct strlist *sl);

/**
 * sl_read_line() - Wrap text into lines and append them to a list.
 * @sl: Destination list.
 * @str: NUL-terminated UTF-8 string to wrap.
 * @wrap: Maximum display width per line.
 *
 * Require SL_USE_CP or SL_USE_SB.
 */
void sl_read_line(struct strlist *sl, const char *str, size_t wrap);

/**
 * sl_to_argv() - Consume a list into a NULL-terminated string array.
 * @sl: List to consume.
 *
 * Require SL_USE_CP or SL_USE_REF. The caller must free the returned array and
 * items.
 *
 * Return: Pointer to the allocated array.
 */
char **sl_to_argv(struct strlist *sl);

#endif /* STRLIST_H */
