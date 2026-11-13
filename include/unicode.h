/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2024, 2025 Jiamu Sun <barroit@linux.com>
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#ifndef UNICODE_H
#define UNICODE_H

#include <stddef.h>

enum uc_mbctype {
	_UC_9CB,  /* continuation bytes */
	_UC_9A,   /* 1-byte */
	_UC_9B,   /* 2-byte */
	_UC_9C,   /* 3-byte */
	_UC_9D,   /* 4-byte */
};

extern unsigned int _uc_mbctype[];

/**
 * uc_force_utf8_ctype() - Configure the character locale or terminate on
 *			   failure.
 */
void uc_force_utf8_ctype(void);

/**
 * macro uc_ismbctype - Test whether a byte matches a character class.
 * @c: Byte to classify.
 * @t: Character class to match.
 *
 * Return: Nonzero on a match, zero otherwise.
 */
#define uc_ismbctype(c, t) (uc_mbctype(c) == t)

/**
 * macro uc_mbctype - Get the UTF-8 byte classification.
 * @c: Byte value to classify.
 *
 * Return: Character class of @c.
 */
#define uc_mbctype(c) _uc_mbctype[(unsigned int)c]

/**
 * macro uc_ismb1 - Test whether a byte represents a single-byte character.
 * @c: Byte to check.
 *
 * Return: Nonzero on a match, zero otherwise.
 */
#define uc_ismb1(c)  uc_ismbctype(c, _UC_9A)

/**
 * macro uc_ismb2 - Test whether a byte has a two-byte leading pattern.
 * @c: Byte to check.
 *
 * Return: Nonzero on a match, zero otherwise.
 */
#define uc_ismb2(c)  uc_ismbctype(c, _UC_9B)

/**
 * macro uc_ismb3 - Test whether a byte has a three-byte leading pattern.
 * @c: Byte to check.
 *
 * Return: Nonzero on a match, zero otherwise.
 */
#define uc_ismb3(c)  uc_ismbctype(c, _UC_9C)

/**
 * macro uc_ismb4 - Test whether a byte has a four-byte leading pattern.
 * @c: Byte to check.
 *
 * Return: Nonzero on a match, zero otherwise.
 */
#define uc_ismb4(c)  uc_ismbctype(c, _UC_9D)

/**
 * macro uc_ismbcb - Test whether a byte is a UTF-8 continuation byte.
 * @c: Byte to check.
 *
 * Return: Nonzero on a match, zero otherwise.
 */
#define uc_ismbcb(c) uc_ismbctype(c, _UC_9CB)

/**
 * uc_iseoc() - Test whether a character is a recognized clause-ending
 *		punctuation mark.
 * @c: Character to check.
 *
 * Return: Nonzero on a match, zero otherwise.
 */
int uc_iseoc(wchar_t c);

/**
 * uc_iswide() - Test whether a Unicode character is wide.
 * @c: Character to check.
 *
 * Return: Nonzero if wide, zero otherwise.
 */
int uc_iswide(wchar_t c);

/**
 * uc_mbslen() - Count characters in a UTF-8 string.
 * @s: NUL-terminated UTF-8 string.
 *
 * Return: Number of characters.
 */
size_t uc_mbslen(const char *s);

/**
 * uc_mbtowc() - Decode a UTF-8 sequence into a wide character.
 * @seq: Pointer to a valid UTF-8 sequence.
 *
 * Return: Decoded character.
 */
wchar_t uc_mbtowc(const char *seq);

#endif /* UNICODE_H */
