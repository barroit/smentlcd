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

/**
 * macro xcalloc - Allocate zero-initialized memory or terminate on failure.
 * @nmemb: Number of elements.
 * @size: Size of each element in bytes.
 *
 * Return: Pointer to the allocated memory.
 */
#define xcalloc(nmemb, size) __xalloc(calloc, nmemb, size)

/**
 * macro xmalloc - Allocate memory or terminate on failure.
 * @size: Number of bytes to allocate.
 *
 * Return: Pointer to the allocated memory.
 */
#define xmalloc(size) __xalloc(malloc, size)

/**
 * macro xmmap - Map memory or terminate on failure.
 * @addr: Requested mapping address.
 * @len: Mapping length in bytes.
 * @prot: Memory protection flags.
 * @flags: Mapping flags.
 * @fildes: File descriptor.
 * @off: File offset.
 *
 * Return: Pointer to the mapped memory.
 */
#define xmmap(addr, len, prot, flags, fildes, off) \
	__xalloc(mmap, addr, len, prot, flags, fildes, off)

/**
 * macro xrealloc - Resize an allocation or terminate on failure.
 * @ptr: Pointer to the existing allocation.
 * @size: New size in bytes.
 *
 * Return: Pointer to the resized memory.
 */
#define xrealloc(ptr, size) __xalloc(realloc, ptr, size)

/**
 * macro xreallocarray - Resize an array or terminate on failure.
 * @ptr: Pointer to the existing allocation.
 * @nmemb: Number of elements.
 * @size: Size of each element in bytes.
 *
 * Return: Pointer to the resized memory.
 */
#define xreallocarray(ptr, nmemb, size) \
	__xalloc(reallocarray, ptr, nmemb, size)

/**
 * macro xstrdup - Duplicate a string or terminate on failure.
 * @s: NUL-terminated string to duplicate.
 *
 * Return: Pointer to the duplicated string.
 */
#define xstrdup(s) __xalloc(strdup, s)

/**
 * macro XREALLOCBUF - Grow an array or terminate on failure.
 * @x: Pointer to the array.
 * @new: Required number of elements.
 * @cap: Current capacity, updated with the new capacity.
 */
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
