/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 *
 * When changing this header, make sure Documentation/lib/attr.rst is up to
 * date.
 */

#ifndef ATTR_H
#define ATTR_H

/*
 * ========================== Optimization ==========================
 */

/**
 * macro attr_always_inline - Require the compiler to inline a function.
 */
#define attr_always_inline inline __attribute__((__always_inline__))

/**
 * macro attr_cold - Mark a function as unlikely to execute.
 *
 * Optimize the function for size.
 */
#define attr_cold __attribute__((__cold__))

/*
 * ======================= Function Properties ======================
 */

/**
 * macro attr_noreturn - Indicate that a function does not return to its caller.
 */
#define attr_noreturn __attribute__((__noreturn__))

/**
 * macro attr_pure - Mark a function that has no side effects.
 *
 * The function may read memory but must not modify it.
 */
#define attr_pure __attribute__((__pure__))

/**
 * macro attr_malloc - Indicate that a function returns unaliased memory.
 */
#define attr_malloc __attribute__((__malloc__))

/*
 * ==================== Initialization & Cleanup ====================
 */

/**
 * macro attr_constructor - Execute a function during program initialization.
 */
#define attr_constructor() __attribute__((__constructor__))

/**
 * macro attr_cleanup - Call a function when a local variable goes out of scope.
 * @x: Cleanup function.
 *
 * The function receives a pointer to the variable.
 */
#define attr_cleanup(x) __attribute__((__cleanup__(x)))

/*
 * ===================== Diagnostics & Analysis =====================
 */

/**
 * macro attr_printf - Enable checking of printf-style format strings.
 * @fmt: Index of the format string argument.
 * @va: Index of the first argument to check.
 */
#define attr_printf(fmt, va) __attribute__((__format__(__printf__, fmt, va)))

/**
 * macro attr_warn_unused_result - Warn when a return value is ignored.
 */
#define attr_warn_unused_result __attribute__((__warn_unused_result__))

#if __has_attribute(__access__)
/**
 * macro attr_read_only - Mark a pointer argument as read-only.
 * @...: Pointer argument index and optional size argument index.
 *
 * This attribute is omitted if the compiler does not support it.
 */
# define attr_read_only(...) \
	 __attribute__((__access__(__read_only__, __VA_ARGS__)))
#else
# define attr_read_only(...)
#endif

/**
 * macro attr_maybe_unused - Suppress warnings about unused declarations.
 */
#define attr_maybe_unused __attribute__((__unused__))

/*
 * ======================= Symbols & sections =======================
 */

/**
 * macro attr_always_used - Require the compiler to emit a function or variable.
 *
 * Prevent the compiler from discarding the declaration as unused.
 */
#define attr_always_used __attribute__((__used__))

/**
 * macro attr_section - Place a function or variable in a specified section.
 * @x: Section name.
 */
#define attr_section(x) __attribute__((__section__(x)))

/**
 * macro attr_weak - Declare a symbol as weak.
 *
 * A strong definition of the same symbol takes precedence at link time.
 */
#define attr_weak __attribute__((weak))

/*
 * ========================= Instrumentation ========================
 */

/**
 * macro attr_no_asan - Disable AddressSanitizer instrumentation for a function.
 */
#define attr_no_asan __attribute__((no_sanitize("address")))

#endif /* ATTR_H */
