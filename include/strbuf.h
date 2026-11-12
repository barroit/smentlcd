/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#ifndef STRBUF_H
#define STRBUF_H

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include "attr.h"

/**
 * macro SB_ALLOC - Enable automatic buffer allocation and growth.
 */
#define SB_ALLOC (1u << 0)

/**
 * macro SB_PREALLOC - Use a preallocated buffer with fixed capacity.
 */
#define SB_PREALLOC (1u << 1)

/**
 * struct strbuf - Mutable string buffer.
 * @buf: Pointer to the buffer storage.
 * @len: String length in bytes, excluding the terminating NUL.
 * @cap: Buffer capacity in bytes, including the terminating NUL.
 * @mode: Buffer mode.
 */
struct strbuf {
	char *buf;
	size_t len;
	size_t cap;
	uint64_t mode;
};

/**
 * macro SB_INIT - Initialize an empty buffer with automatic growth.
 */
#define SB_INIT() { .buf = NULL, .len = 0, .cap = 0, .mode = SB_ALLOC }

/**
 * macro SB_INIT_PREALLOC - Initialize an empty buffer with fixed capacity.
 * @__buf: Preallocated buffer storage.
 * @__cap: Buffer capacity in bytes.
 */
#define SB_INIT_PREALLOC(__buf, __cap) \
	{ .buf = __buf, .len = 0, .cap = __cap, .mode = SB_PREALLOC }

/**
 * sb_init() - Initialize an empty buffer with the specified mode.
 * @sb: Buffer to initialize.
 * @mode: Buffer mode.
 */
void sb_init(struct strbuf *sb, uint64_t mode);

/**
 * sb_destroy() - Free buffer storage and clear its pointer.
 * @sb: Buffer to destroy.
 *
 * Do not call on buffers backed by stack or static storage.
 */
void sb_destroy(struct strbuf *sb);

/**
 * sb_reinit() - Free buffer storage and reinitialize with a new mode.
 * @sb: Buffer to reinitialize.
 * @mode: New buffer mode.
 *
 * Do not call on buffers backed by stack or static storage.
 */
void sb_reinit(struct strbuf *sb, uint64_t mode);

/**
 * sb_chop() - Remove bytes from the end of a buffer.
 * @sb: Buffer to modify.
 * @len: Number of bytes to remove.
 *
 * @len must not exceed the current buffer length.
 */
void sb_chop(struct strbuf *sb, size_t len);

/**
 * sb_trunc() - Truncate a buffer to a specified length.
 * @sb: Buffer to modify.
 * @len: New buffer length in bytes.
 *
 * @len must not exceed the current buffer length.
 */
void sb_trunc(struct strbuf *sb, size_t len);

/**
 * macro sb_write_ch - Append a character to a buffer.
 * @sb: Destination buffer.
 * @c: Character to append.
 *
 * Return: 1 on success, or 0 if capacity is insufficient.
 */
#define sb_write_ch(sb, c) sb_write_ch_at(sb, (sb)->len, c)

/**
 * macro sb_write_str - Append a NUL-terminated string to a buffer.
 * @sb: Destination buffer.
 * @s: String to append.
 *
 * Return: Number of bytes written, or 0 if capacity is insufficient.
 */
#define sb_write_str(sb, s) sb_write_str_at(sb, (sb)->len, s)

/**
 * macro sb_vwritef - Append a formatted string to a buffer.
 * @sb: Destination buffer.
 * @fmt: printf-style format string.
 * @ap: Argument list.
 *
 * Return: Number of bytes written, or 0 if capacity is insufficient.
 */
#define sb_vwritef(sb, fmt, ap) sb_vwritef_at(sb, (sb)->len, fmt, ap)

/**
 * macro sb_writef - Append a formatted string to a buffer.
 * @sb: Destination buffer.
 * @fmt: printf-style format string.
 * @...: Format arguments.
 *
 * Return: Number of bytes written, or 0 if capacity is insufficient.
 */
#define sb_writef(sb, fmt, ...) sb_writef_at(sb, (sb)->len, fmt, ##__VA_ARGS__)

/**
 * sb_write_ch_at() - Write a character at @pos and truncate subsequent content.
 * @sb: Destination buffer.
 * @pos: Write position, at most the current buffer length.
 * @c: Character to write.
 *
 * Return: 1 on success, or 0 if capacity is insufficient.
 */
size_t sb_write_ch_at(struct strbuf *sb, size_t pos, char c);

/**
 * sb_write_str_at() - Write a string at @pos and truncate subsequent content.
 * @sb: Destination buffer.
 * @pos: Write position, at most the current buffer length.
 * @s: NUL-terminated string to write.
 *
 * Return: Number of bytes written, or 0 if capacity is insufficient.
 */
size_t sb_write_str_at(struct strbuf *sb, size_t pos, const char *s);

/**
 * sb_vwritef_at() - Write a formatted string at @pos and truncate the buffer
 *		    after the written data.
 * @sb: Destination buffer.
 * @pos: Write position, at most the current buffer length.
 * @fmt: printf-style format string.
 * @ap: Argument list.
 *
 * Return: Number of bytes written, or 0 if capacity is insufficient.
 */
size_t sb_vwritef_at(struct strbuf *sb,
		     size_t pos, const char *fmt, va_list ap);

/**
 * sb_writef_at() - Write a formatted string at @pos and truncate the buffer
 *		   after the written data.
 * @sb: Destination buffer.
 * @pos: Write position, at most the current buffer length.
 * @fmt: printf-style format string.
 * @...: Format arguments.
 *
 * Return: Number of bytes written, or 0 if capacity is insufficient.
 */
size_t sb_writef_at(struct strbuf *sb,
		    size_t pos, const char *fmt, ...) attr_printf(3, 4);

#endif /* STRBUF_H */
