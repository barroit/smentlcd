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

#define SB_ALLOC    (1u << 0)
#define SB_PREALLOC (1u << 1)

struct strbuf {
	char *buf;
	size_t len;
	size_t cap;
	uint64_t mode;
};

#define SB_INIT { .buf = NULL, .len = 0, .cap = 0, .mode = SB_ALLOC }
#define SB_INIT_PREALLOC(__buf, __cap) \
	{ .buf = __buf, .len = 0, .cap = __cap, .mode = SB_PREALLOC }

void sb_init(struct strbuf *sb, uint64_t mode);

void sb_destroy(struct strbuf *sb);

void sb_reinit(struct strbuf *sb, uint64_t mode);

void sb_chop(struct strbuf *sb, size_t len);

void sb_trunc(struct strbuf *sb, size_t len);

#define sb_write_ch(sb, c) sb_write_ch_at(sb, (sb)->len, c)

#define sb_write_str(sb, s) sb_write_str_at(sb, (sb)->len, s)

#define sb_vwritef(sb, fmt, ap) sb_vwritef_at(sb, (sb)->len, fmt, ap)

#define sb_writef(sb, fmt, ...) sb_writef_at(sb, (sb)->len, fmt, ##__VA_ARGS__)

size_t sb_write_ch_at(struct strbuf *sb, size_t pos, char c);

size_t sb_write_str_at(struct strbuf *sb, size_t pos, const char *s);

size_t sb_vwritef_at(struct strbuf *sb,
		     size_t pos, const char *fmt, va_list ap);

size_t sb_writef_at(struct strbuf *sb,
		    size_t pos, const char *fmt, ...) attr_printf(3, 4);

#endif /* STRBUF_H */
