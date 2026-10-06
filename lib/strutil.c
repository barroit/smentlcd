// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#include <stddef.h>
#include <string.h>

#include "strutil.h"

const char *str_skip(const char *s1, const char *s2)
{
	do {
		if (!s2[0])
			return (char *)s1;
	} while (*s1++ == *s2++);

	return NULL;
}

int str_skip2(const char *s1, const char *s2, const char **__res)
{
	const char *res = str_skip(s1, s2);

	if (res == NULL)
		return -1;

	*__res = res;
	return 0;
}

const char *str_seek_suffix(const char *str, char after)
{
	const char *ret;

	ret = strrchr(str, after);
	if (!ret[0] || !ret[1])
		return NULL;

	return &ret[1];
}
