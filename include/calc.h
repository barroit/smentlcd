/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#ifndef CALC_H
#define CALC_H

#include <inttypes.h>
#include <limits.h>
#include <stdint.h>

#include "attr.h"
#include "compiler.h"
#include "log.h"

/**
 * macro sizeof_array - Count the elements of an array.
 * @x: Array to inspect.
 *
 * Reject non-array arguments at compile time.
 *
 * Return: Number of elements.
 */
#define sizeof_array(x) (__sizeof_array(x) + assert_array(x))
#define __sizeof_array(x) (sizeof(x) / sizeof((x)[0]))

/**
 * macro bitsof - Calculate the size of an object in bits.
 * @x: Complete object type or expression.
 *
 * Return: Size in bits.
 */
#define bitsof(x) (CHAR_BIT * sizeof(x))

/**
 * macro maxof - Get the maximum value representable by an integer type.
 * @x: Integer type or expression.
 *
 * The type must not be wider than ``uintmax_t``.
 *
 * Return: Maximum representable value.
 */
#define maxof(x) \
	((UINTMAX_MAX >> type_is_signed(x)) >> (bitsof(uintmax_t) - bitsof(x)))

/**
 * macro mult_is_overflow - Check whether integer multiplication overflows.
 * @a: First operand.
 * @b: Second operand.
 *
 * Require unsigned or nonnegative signed operands. The type of @a must
 * not be wider than ``uintmax_t``.
 *
 * Return: Nonzero on overflow, zero otherwise.
 */
#define mult_is_overflow(a, b) ((a) && (b) > (maxof(a) / (a)))

/**
 * macro add_is_overflow - Check whether integer addition overflows.
 * @a: First operand.
 * @b: Second operand.
 *
 * Require unsigned or nonnegative signed operands. The type of @a must
 * not be wider than ``uintmax_t``.
 *
 * Return: Nonzero on overflow, zero otherwise.
 */
#define add_is_overflow(a, b) ((b) > (maxof(a) - (a)))

/**
 * macro mult_or_die - Multiply two integers or abort on overflow.
 * @a: First operand.
 * @b: Second operand.
 *
 * Require unsigned or nonnegative signed operands. The type of @a must
 * not be wider than ``uintmax_t``.
 *
 * Return: Product of @a and @b.
 */
#define mult_or_die(a, b)			\
({						\
	if (unlikely(mult_is_overflow(a, b)))	\
		bug("%" PRIuMAX " * %" PRIuMAX	\
		    " overflows in %zu-byte",	\
		    (a), (b), sizeof(a));	\
	(a) * (b);				\
})

/**
 * macro add_or_die - Add two integers or abort on overflow.
 * @a: First operand.
 * @b: Second operand.
 *
 * Require unsigned or nonnegative signed operands. The type of @a must
 * not be wider than ``uintmax_t``.
 *
 * Return: Sum of @a and @b.
 */
#define add_or_die(a, b)			\
({						\
	if (unlikely(add_is_overflow(a, b)))	\
		bug("%" PRIuMAX " + %" PRIuMAX	\
		    " overflows in %zu-byte",	\
		    (a), (b), sizeof(a));	\
	(a) + (b);				\
})

/**
 * macro next_buf_size - Calculate the next buffer capacity.
 * @x: Current capacity.
 *
 * Return: Calculated capacity..
 */
#define next_buf_size(x) cc_align_down((((x) + 8) * 3) >> 1, 8)

/**
 * macro next_arr_size - Calculate the next array capacity.
 * @x: Current capacity.
 *
 * Return: Calculated capacity.
 */
#define next_arr_size(x) cc_align_down((x) + ((x) >> 3) + 6, 8)

/**
 * macro min - Get the smaller of two values.
 * @a: First value.
 * @b: Second value.
 *
 * Return: The smaller value.
 */
#define min(a, b) ((a) < (b) ? (a) : (b))

/**
 * macro max - Get the larger of two values.
 * @a: First value.
 * @b: Second value.
 *
 * Return: The larger value.
 */
#define max(a, b) ((a) > (b) ? (a) : (b))

#endif /* CALC_H */
