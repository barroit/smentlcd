/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#ifndef XALLOC_H
#define XALLOC_H

#include <sys/types.h>

#include "attr.h"
#include "calc.h"
#include "compiler.h"

#define xcalloc(...)       __xalloc(calloc, __VA_ARGS__)
#define xmalloc(...)       __xalloc(malloc, __VA_ARGS__)
#define xmmap(...)         __xalloc(mmap, __VA_ARGS__)
#define xrealloc(...)      __xalloc(realloc, __VA_ARGS__)
#define xreallocarray(...) __xalloc(reallocarray, __VA_ARGS__)
#define xstrdup(...)       __xalloc(strdup, __VA_ARGS__)

#define XREALLOCBUF(x, new, cap)			\
do {							\
	if (assert_diff_type(*(x), void));		\
	if (assert_diff_type(*(x), void *));		\
	cap = next_buf_size(cap);			\
	if (cap < (new))				\
		cap = next_buf_size(new);		\
	x = xreallocarray(x, sizeof(*(x)), cap);	\
} while (0)

#define __xalloc(name, ...) \
	__x ## name(__FILE__, __LINE__, __func__, __VA_ARGS__)

void *__xcalloc(const char *file, int line, const char *func, size_t nmemb,
		size_t size);

void *__xmalloc(const char *file, int line, const char *func, size_t size);

void *__xmmap(const char *file, int line, const char *func, void *addr,
	      size_t len, int prot, int flags, int fildes, off_t off);

void *__xrealloc(const char *file, int line, const char *func, void *ptr,
		 size_t size);

void *__xreallocarray(const char *file, int line, const char *func, void *ptr,
		      size_t nmemb, size_t size);

char *__xstrdup(const char *file, int line, const char *func, const char *s);

#endif /* XALLOC_H */
