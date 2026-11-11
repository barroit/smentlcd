/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#ifndef COMPILER_H
#define COMPILER_H

#ifdef CC_HAS_BUILTIN_ALIGN_UP
# define cc_align_up __builtin_align_up
#else
# define cc_align_up(m, n) (__cc_align_up(m, n) + assert_pow2(n))
# define __cc_align_up(m, n) (((m) + (n) - 1) & ~(uintmax_t)(n - 1))
#endif

#ifdef CC_HAS_BUILTIN_ALIGN_DOWN
# define cc_align_down __builtin_align_down
#else
# define cc_align_down(m, n) (__cc_align_down(m, n) + assert_pow2(n))
# define __cc_align_down(m, n) ((m) & ~(uintmax_t)(n - 1))
#endif

#define cc_alignof __alignof__

#define cc_expect __builtin_expect

#define cc_offsetof __builtin_offsetof

#define cc_popcount __builtin_popcount

#define cc_trap __builtin_trap

#define cc_types_compatible_p __builtin_types_compatible_p

#define cc_unreachable __builtin_unreachable

#define likely(x)   cc_expect(!!(x), 1)
#define unlikely(x) cc_expect(!!(x), 0)

#define BUG_ON(x) if (BUG_ON_EXPR(x))
#define BUG_ON_EXPR(x) (sizeof(struct { int:(-!!(x)); }) & 0)

#define type_is_same(a, b) cc_types_compatible_p(typeof(a), typeof(b))

#define type_is_signed(x) ((typeof(x))-1 < 0)

#define assert_same_type(a, b) BUG_ON_EXPR(!type_is_same(a, b))

#define assert_diff_type(a, b) BUG_ON_EXPR(type_is_same(a, b))

#define assert_array(x) BUG_ON_EXPR(type_is_same(x, &x[0]))

#define assert_pow2(x) BUG_ON_EXPR(!(x) || ((x) & ((x) - 1)))

#define assert_ascii(x) BUG_ON_EXPR(((unsigned char)x) & 0x80)

#define container_of(x, type, memb)				\
({								\
	BUG_ON(!type_is_same(*(x), void) &&			\
	       !type_is_same(*(x), ((type *)0)->memb));		\
	(type *)((void *)(x) - cc_offsetof(type, memb));	\
})

#endif /* COMPILER_H */
