/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#ifndef COMPILER_H
#define COMPILER_H

#ifdef CC_HAS_BUILTIN_ALIGN_UP
/**
 * macro cc_align_up - Align a value upward to a multiple.
 * @m: Value to align.
 * @n: Alignment, a power of two.
 *
 * Return: The aligned value.
 */
# define cc_align_up(m, n) __builtin_align_up(m, n)
#else
# define cc_align_up(m, n) (__cc_align_up(m, n) + assert_pow2(n))
# define __cc_align_up(m, n) (((m) + (n) - 1) & ~(uintmax_t)(n - 1))
#endif

#ifdef CC_HAS_BUILTIN_ALIGN_DOWN
/**
 * macro cc_align_down - Align a value downward to a multiple.
 * @m: Value to align.
 * @n: Alignment, a power of two.
 *
 * Return: The aligned value.
 */
# define cc_align_down(m, n) __builtin_align_down(m, n)
#else
# define cc_align_down(m, n) (__cc_align_down(m, n) + assert_pow2(n))
# define __cc_align_down(m, n) ((m) & ~(uintmax_t)(n - 1))
#endif

/**
 * macro cc_alignof - Return the alignment of a type or expression.
 * @type: Type or expression to inspect.
 *
 * Return: The alignment requirement in bytes.
 */
#define cc_alignof(type) __alignof__(type)

/**
 * macro cc_expect - Provide the compiler with an expected value.
 * @x: Expression to evaluate.
 * @val: Expected value.
 *
 * Return: The value of @x.
 */
#define cc_expect(x, val) __builtin_expect(x, val)

/**
 * macro cc_offsetof - Return the byte offset of a structure member.
 * @type: Structure type.
 * @memb: Member name.
 *
 * Return: The offset in bytes.
 */
#define cc_offsetof(type, memb) __builtin_offsetof(type, memb)

/**
 * macro cc_popcount - Count the set bits in an integer.
 * @x: Integer to inspect.
 *
 * Return: The number of set bits.
 */
#define cc_popcount(x) __builtin_popcount(x)

/**
 * macro cc_trap - Terminate execution using a compiler-generated trap.
 */
#define cc_trap() __builtin_trap()

/**
 * macro cc_types_compatible_p - Test whether two types are compatible.
 * @a: First type.
 * @b: Second type.
 *
 * Return: Nonzero if the types are compatible, zero otherwise.
 */
#define cc_types_compatible_p(a, b) __builtin_types_compatible_p(a, b)

/**
 * macro cc_unreachable - Mark a code path as unreachable.
 */
#define cc_unreachable() __builtin_unreachable()

/**
 * macro likely - Indicate that an expression is expected to be true.
 * @x: Expression to evaluate.
 *
 * Return: The boolean value of @x.
 */
#define likely(x)   cc_expect(!!(x), 1)

/**
 * macro unlikely - Indicate that an expression is expected to be false.
 * @x: Expression to evaluate.
 *
 * Return: The boolean value of @x.
 */
#define unlikely(x) cc_expect(!!(x), 0)

/**
 * macro BUG_ON - Trigger a compilation error if a condition is true.
 * @x: Compile-time condition.
 */
#define BUG_ON(x) if (BUG_ON_EXPR(x))

/**
 * macro BUG_ON_EXPR - Trigger a compilation error within an expression.
 * @x: Compile-time condition.
 *
 * Return: Zero if @x is false.
 */
#define BUG_ON_EXPR(x) (sizeof(struct { int:(-!!(x)); }) & 0)

/**
 * macro type_is_same - Test whether two expressions have compatible types.
 * @a: First expression.
 * @b: Second expression.
 *
 * Return: Nonzero if the types are compatible, zero otherwise.
 */
#define type_is_same(a, b) cc_types_compatible_p(typeof(a), typeof(b))

/**
 * macro type_is_signed - Test whether an expression has a signed type.
 * @x: Expression to check.
 *
 * Return: Nonzero if the type is signed, zero otherwise.
 */
#define type_is_signed(x) ((typeof(x))-1 < 0)

/**
 * macro assert_same_type - Require two expressions to have compatible types.
 * @a: First expression.
 * @b: Second expression.
 */
#define assert_same_type(a, b) BUG_ON_EXPR(!type_is_same(a, b))

/**
 * macro assert_diff_type - Require two expressions to have incompatible types.
 * @a: First expression.
 * @b: Second expression.
 */
#define assert_diff_type(a, b) BUG_ON_EXPR(type_is_same(a, b))

/**
 * macro assert_array - Require an expression to be an array.
 * @x: Expression to check.
 */
#define assert_array(x) BUG_ON_EXPR(type_is_same(x, &x[0]))

/**
 * macro assert_pow2 - Require a nonzero power-of-two constant.
 * @x: Value to check.
 */
#define assert_pow2(x) BUG_ON_EXPR(!(x) || ((x) & ((x) - 1)))

/**
 * macro assert_ascii - Require the highest bit of a byte to be clear.
 * @x: Value to check.
 */
#define assert_ascii(x) BUG_ON_EXPR(((unsigned char)x) & 0x80)

/**
 * macro container_of - Get the containing structure from a member pointer.
 * @x: Pointer to the member.
 * @type: Type of the containing structure.
 * @memb: Name of the member.
 *
 * The pointer type must match the member type or be ``void *``.
 *
 * Return: Pointer to the containing structure.
 */
#define container_of(x, type, memb)				\
({								\
	BUG_ON(!type_is_same(*(x), void) &&			\
	       !type_is_same(*(x), ((type *)0)->memb));		\
	(type *)((void *)(x) - cc_offsetof(type, memb));	\
})

#endif /* COMPILER_H */
