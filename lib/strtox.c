// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stddef.h>

#include "strtox.h"

static inline int has_hex_prefix(const char *str)
{
	return str[0] == '0' && tolower(str[1]) == 'x';
}

static int probe_radix(const char *str)
{
	if (has_hex_prefix(str))
		return 16;
	else if (str[0] == '0')
		return 8;
	else
		return 10;
}

static inline int has_sign(const char *str)
{
	return str[0] == '-' || str[0] == '+';
}

static char probe_sign(const char *str)
{
	if (str[0] == '-')
		return '-';
	else
		return '+';
}

static int __parse_integer(const char *__str, int base, size_t limit,
			   unsigned long long *__res)
{
	const char *str = __str;
	unsigned long long res = 0;

	for (; limit && *str; limit--, str++) {
		char c = *str;
		char lc = tolower(c);
		char val;

		if ('0' <= c && c <= '9')
			val = c - '0';
		else if ('a' <= lc && lc <= 'z')
			val = lc - 'a' + 10;
		else
			break;

		if (val >= base)
			break;

		if (res & (-1ull << 60)) {
			if (res > (ULLONG_MAX - val) / base) {
				errno = ERANGE;
				return -1;
			}
		}

		res = res * base + val;
	}

	if (str == __str || *str) {
		errno = EINVAL;
		return -1;
	}

	*__res = res;
	return 0;
}

static int parse_integer(const char *str, int base, unsigned long long *res,
			 char *__sign)
{
	char sign = probe_sign(str);
	int err;

	if (has_sign(str))
		str++;

	if (base == 0)
		base = probe_radix(str);

	if (base == 16 && has_hex_prefix(str))
		str += 2;

	err = __parse_integer(str, base, -1, res);

	if (err)
		return err;

	*__sign = sign;
	return 0;
}

#define __strtox_unsigned(str, base, res)			\
({								\
	char sign;						\
	unsigned long long __res;				\
	int err = parse_integer(str, base, &__res, &sign);	\
								\
	if (!err && __res != (typeof(*(res)))__res) {		\
		errno = ERANGE;					\
		err = -1;					\
	}							\
								\
	if (!err) {						\
		if (sign == '-')				\
			__res = -__res;				\
		*res = __res;					\
	}							\
								\
	err;							\
})

#define __strtox_signed(str, base, res)				\
({								\
	char sign;						\
	unsigned long long __res;				\
	int err = parse_integer(str, base, &__res, &sign);	\
								\
	if (!err && ((sign == '-' && (long long)-__res > 0) ||	\
		     (sign == '+' && (long long)__res < 0))) {	\
		errno = ERANGE;					\
		err = -1;					\
	}							\
								\
	if (!err) {						\
		if (sign == '-')				\
			__res = -__res;				\
								\
		if ((long long)__res !=				\
		    (typeof(*(res)))(long long)__res) {		\
			errno = ERANGE;				\
			err = -1;				\
		}						\
	}							\
								\
	if (!err)						\
		*res = __res;					\
								\
	err;							\
})

int strxtoull(const char *str, int base, unsigned long long *res)
{
	return __strtox_unsigned(str, base, res);
}

int strxtoll(const char *str, int base, long long *res)
{
	return __strtox_signed(str, base, res);
}

int strxtoul(const char *str, int base, unsigned long *res)
{
	return __strtox_unsigned(str, base, res);
}

int strxtou(const char *str, int base, unsigned int *res)
{
	return __strtox_unsigned(str, base, res);
}

int strxtol(const char *str, int base, long *res)
{
	return __strtox_signed(str, base, res);
}

int strxtos(const char *str, int base, int *res)
{
	return __strtox_signed(str, base, res);
}

int strxtou64(const char *str, int base, uint64_t *res)
{
	return __strtox_unsigned(str, base, res);
}

int strxtou32(const char *str, int base, uint32_t *res)
{
	return __strtox_unsigned(str, base, res);
}

int strxtou16(const char *str, int base, uint16_t *res)
{
	return __strtox_unsigned(str, base, res);
}

int strxtou8(const char *str, int base, uint8_t *res)
{
	return __strtox_unsigned(str, base, res);
}

int strxtos64(const char *str, int base, int64_t *res)
{
	return __strtox_signed(str, base, res);
}

int strxtos32(const char *str, int base, int32_t *res)
{
	return __strtox_signed(str, base, res);
}

int strxtos16(const char *str, int base, int16_t *res)
{
	return __strtox_signed(str, base, res);
}

int strxtos8(const char *str, int base, int8_t *res)
{
	return __strtox_signed(str, base, res);
}
