/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#ifndef STRUTIL_H
#define STRUTIL_H

/*
 * Skip s2 if it is a prefix of s1. Return the remainder of s1 on a match,
 * or NULL otherwise. An empty s2 matches and returns s1.
 */
const char *str_skip(const char *s1, const char *s2);

/*
 * Like strskip(), but return 0 and store the remainder in *res on a match.
 * Return -1 on a mismatch without changing *res.
 */
int str_skip2(const char *s1, const char *s2, const char **res);

const char *str_seek_suffix(const char *filename, char after);

#endif /* STRUTIL_H */
