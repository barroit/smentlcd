// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#include <errno.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include "calc.h"
#include "cpp.h"
#include "log.h"
#include "patch/reallocarray.h"
#include "xalloc.h"

static inline void __assert_allocated(const char *file, int line,
				      const char *func, void *ptr,
				      void *invalid, size_t n)
{
	if (ptr != invalid)
		return;

	log_writef(STDERR_FILENO, NULL, NULL,
		   "%s:%d,%s(): out of memory (tried to allocate %zu bytes)",
		   file, line, func, n);
	abort();
}

static inline void assert_allocated(const char *file, int line,
				    const char *func, void *ptr, size_t n)
{
	__assert_allocated(file, line, func, ptr, NULL, n);
}

#ifdef CONFIG_WARN_LARGE_ALLOC
static inline void warn_large_alloc(const char *file, int line,
				    const char *func, uintmax_t n)
{
	if (n < CONFIG_LARGE_ALLOC)
		return;

	log_writef(STDERR_FILENO, NULL, NULL,
		   "%s:%d,%s(): allocated %" PRIuMAX " bytes exceeds %" PRIuMAX,
		   file, line, func, n, (uintmax_t)CONFIG_LARGE_ALLOC);
}
#else
# define warn_large_alloc NOOP
#endif

void *__xcalloc(const char *file, int line, const char *func, size_t nmemb,
		size_t size)
{
	size_t nalloc = mult_or_die(nmemb, size);
	void *buf = calloc(nmemb, size);

	assert_allocated(file, line, func, buf, nalloc);
	warn_large_alloc(file, line, func, nalloc);
	return buf;
}

void *__xmalloc(const char *file, int line, const char *func, size_t size)
{
	void *buf = malloc(size);

	assert_allocated(file, line, func, buf, size);
	warn_large_alloc(file, line, func, size);
	return buf;
}

void *__xmmap(const char *file, int line, const char *func, void *addr,
	      size_t len, int prot, int flags, int fildes, off_t off)
{
	void *buf = mmap(addr, len, prot, flags, fildes, off);

	__assert_allocated(file, line, func, buf, MAP_FAILED, len);
	return buf;
}

void *__xrealloc(const char *file, int line, const char *func, void *ptr,
		 size_t size)
{
	void *buf = realloc(ptr, size);

	assert_allocated(file, line, func, buf, size);
	warn_large_alloc(file, line, func, size);
	return buf;
}

void *__xreallocarray(const char *file, int line, const char *func, void *ptr,
		      size_t nmemb, size_t size)
{
	uintmax_t nalloc = mult_or_die(nmemb, size);
	void *buf = reallocarray(ptr, nmemb, size);

	assert_allocated(file, line, func, buf, nalloc);
	warn_large_alloc(file, line, func, nalloc);
	return buf;
}

char *__xstrdup(const char *file, int line, const char *func, const char *s)
{
	char *buf = strdup(s);

	if (buf)
		return buf;

	log_writef(STDERR_FILENO, NULL, NULL,
		   "%s:%d,%s(): failed to duplicate string '%.10s...'",
		   file, line, func, s);
	abort();
}
