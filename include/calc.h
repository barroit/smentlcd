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

#define sizeof_array(x) (__sizeof_array(x) + assert_array(x))
#define __sizeof_array(x) (sizeof(x) / sizeof((x)[0]))

/*
 * domain: any complete object type or expression
 */
#define bitsof(x) (CHAR_BIT * sizeof(x))

/*
 * domain: any integer type or expression where sizeof(x) <= sizeof(uintmax_t)
 */
#define maxof(x) \
	((UINTMAX_MAX >> type_is_signed(x)) >> (bitsof(uintmax_t) - bitsof(x)))

/*
 * domain: unsigned integer types (or non-negative signed integers) where
 *         sizeof(a) <= sizeof(uintmax_t)
 */
#define mult_is_overflow(a, b) ((a) && (b) > (maxof(a) / (a)))
#define add_is_overflow(a, b) ((b) > (maxof(a) - (a)))

#define mult_or_die(a, b)			\
({						\
	if (unlikely(mult_is_overflow(a, b)))	\
		bug("%" PRIuMAX " * %" PRIuMAX	\
		    " overflows in %zu-byte",	\
		    (a), (b), sizeof(a));	\
	(a) * (b);				\
})

#define add_or_die(a, b)			\
({						\
	if (unlikely(add_is_overflow(a, b)))	\
		bug("%" PRIuMAX " + %" PRIuMAX	\
		    " overflows in %zu-byte",	\
		    (a), (b), sizeof(a));	\
	(a) + (b);				\
})

#define next_buf_size(x) cc_align_down((((x) + 8) * 3) >> 1, 8)
#define next_arr_size(x) cc_align_down((x) + ((x) >> 3) + 6, 8)

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

#endif /* CALC_H */
