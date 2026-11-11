/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#ifndef CPP_H
#define CPP_H

#define ARGC(...) __ARGC(, ##__VA_ARGS__, \
			 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, \
			 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, \
			 13, 12, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1, 0)
#define __ARGC(...) ___ARGC(__VA_ARGS__)
#define ___ARGC(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, \
		_15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, \
		_27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, \
		_39, _40, n, ...) n

#define CALL(name, ...) __CALL(name, ARGC(__VA_ARGS__), ##__VA_ARGS__)
#define __CALL(name, ...) ___CALL(name, __VA_ARGS__)
#define ___CALL(name, argc, ...) name##argc(__VA_ARGS__)

#define NOOP(...) CALL(__NOOP_, ##__VA_ARGS__)
#define __NOOP_0(...) __NOOP_1(0)
#define __NOOP_1(...) __NOOP_2(0, __VA_ARGS__)
#define __NOOP_2(...) __NOOP_3(0, __VA_ARGS__)
#define __NOOP_3(...) __NOOP_4(0, __VA_ARGS__)
#define __NOOP_4(...) __NOOP_5(0, __VA_ARGS__)
#define __NOOP_5(...) __NOOP_6(0, __VA_ARGS__)
#define __NOOP_6(_1, _2, _3, _4, _5, _6) \
	do { if ((_1) && (_2) && (_3) && (_4) && (_5) && (_6)); } while (0)

#define CONCAT(a, b)   __CONCAT(a, b)
#define __CONCAT(a, b) a ## b

#define IS_ENABLED(x)     __IS_ENABLED(CONCAT(__kconfig_enabled_, x), 0)
#define __IS_ENABLED(...) ___IS_ENABLED(__VA_ARGS__)
#define ___IS_ENABLED(_, x, ...) x
#define __kconfig_enabled_1 , 1

#define POISON_ADDR_1 (void *)0x3901
#define POISON_ADDR_2 (void *)0x3902
#define POISON_ADDR_3 (void *)0x3903
#define POISON_ADDR_4 (void *)0x3904
#define POISON_ADDR_5 (void *)0x3905
#define POISON_ADDR_6 (void *)0x3906

#endif /* CPP_H */
