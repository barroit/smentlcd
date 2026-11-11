/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#ifndef ATTR_H
#define ATTR_H

/*
 * ========================== Optimization ==========================
 */

#define attr_always_inline inline __attribute__((__always_inline__))

#define attr_cold __attribute__((__cold__))

/*
 * ======================= Function Properties ======================
 */

#define attr_noreturn __attribute__((__noreturn__))

#define attr_pure __attribute__((__pure__))

#define attr_malloc __attribute__((__malloc__))

/*
 * ==================== Initialization & Cleanup ====================
 */

#define attr_constructor __attribute__((__constructor__))

#define attr_cleanup(x) __attribute__((__cleanup__(x)))

/*
 * ===================== Diagnostics & Analysis =====================
 */

#define attr_printf(fmt, va) __attribute__((__format__(__printf__, fmt, va)))

#define attr_warn_unused_result __attribute__((__warn_unused_result__))

#if __has_attribute(__access__)
# define attr_read_only(...) \
	 __attribute__((__access__(__read_only__, __VA_ARGS__)))
#else
# define attr_read_only(...)
#endif

#define attr_maybe_unused __attribute__((__unused__))

/*
 * ======================= Symbols & sections =======================
 */

#define attr_always_used __attribute__((__used__))

#define attr_section(x) __attribute__((__section__(x)))

#define attr_weak __attribute__((weak))

/*
 * ========================= Instrumentation ========================
 */

#define attr_no_asan __attribute__((no_sanitize("address")))

#endif /* ATTR_H */
