/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#ifndef STRUTIL_H
#define STRUTIL_H

/**
 * str_skip() - Skip a matching prefix from a string.
 * @s1: String to inspect.
 * @s2: Prefix to match.
 *
 * An empty @s2 always matches.
 *
 * Return: Pointer to the remainder of @s1, or NULL on mismatch.
 */
const char *str_skip(const char *s1, const char *s2);

/**
 * str_skip2() - Skip a matching prefix and store the remainder.
 * @s1: String to inspect.
 * @s2: Prefix to match.
 * @res: Pointer to receive the remainder.
 *
 * An empty @s2 always matches. Leave @res unchanged on mismatch.
 *
 * Return: Zero on success, or -1 on mismatch.
 */
int str_skip2(const char *s1, const char *s2, const char **res);

/**
 * str_seek_suffix() - Find the suffix after the last occurrence of a character.
 * @filename: String to inspect.
 * @after: Character preceding the suffix.
 *
 * Return: Pointer to the suffix, or NULL if not found or empty.
 */
const char *str_seek_suffix(const char *filename, char after);

#endif /* STRUTIL_H */
