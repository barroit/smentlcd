/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#ifndef UNITEST_H
#define UNITEST_H

#include <stddef.h>

#include "attr.h"
#include "cpp.h"

enum unitest_directive {
	UNITEST_TODO = 1,
	UNITEST_SKIP,
};

struct unitest {
	const char *detail;
	enum unitest_directive direct;
	const char *direct_reason;
};

typedef void (*unitest_hook_t)(void);
typedef int (*unitest_routine_t)(struct unitest *);

extern unitest_routine_t unitest_begin[];
extern unitest_routine_t unitest_end[];

extern unitest_hook_t unitest_setup;
extern unitest_hook_t unitest_teardown;

#define unitest_routine(name) __ut_routine(__test_ ## name)
#define __ut_routine(name)						\
	static int _ ## name(struct unitest *);				\
	static __ut_ld_sec int (*name)(struct unitest *) = &_ ## name;	\
	static int _ ## name(struct unitest *unitest)

#define unitest_begin(...)					\
	unitest_routine_t __ut_ld_sec unitest_begin[1] = { 0 };	\
	CALL(__ut_begin_, ##__VA_ARGS__)

#define __ut_begin_0() unitest_hook_t unitest_setup = NULL

#define __ut_begin_1(_)					\
	static void __ut_setup(void);			\
	unitest_hook_t unitest_setup = &__ut_setup;	\
	static void __ut_setup(void)

#define unitest_end(...)					\
	unitest_routine_t __ut_ld_sec unitest_end[1] = { 0 };	\
	CALL(__ut_end_, ##__VA_ARGS__)

#define __ut_end_0() \
	unitest_hook_t unitest_teardown = NULL

#define __ut_end_1(_) \
	static void __ut_teardown(void);		\
	unitest_hook_t unitest_teardown = &__ut_teardown;	\
	static void __ut_teardown(void)

#define __ut_ld_sec attr_section(__ut_ld_sec_name) __ut_no_asan attr_always_used

#ifdef __APPLE__
# define __ut_ld_sec_name "__DATA,__miku_test"
#else
# define __ut_ld_sec_name ".miku_test"
#endif

#ifdef CC_HAS_VARIABLE_NO_SANITIZE
# define __ut_no_asan attr_no_asan
#else
# define __ut_no_asan
#endif

#endif /* UNITEST_H */
