/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#ifndef ANSI_H
#define ANSI_H

#include "cpp.h"

/*
 * \033[<style>;<foreground>;<background>m
 * \033[<style>;<color>m
 * \033[<style|color>m
 *
 * Style			Foreground		Background
 * 0	Reset			30    Black		40	Black
 * 1	Bold/bright		31    Red		41	Red
 * 2	Faint			32    Green		42	Green
 * 3	Italic			33    Yellow		43	Yellow
 * 4	Underline		34    Blue		44	Blue
 * 5	Blink			35    Magenta		45	Magenta
 * 7	Inverse			36    Cyan		46	Cyan
 * 22	Normal intensity	37    White		47	White
 * 24	No underline		90    Bright Black	100	Bright Black
 * 25	No blink		91    Bright Red	101	Bright Red
 *				92    Bright Green	102	Bright Green
 *				93    Bright Yellow	103	Bright Yellow
 *				94    Bright Blue	104	Bright Blue
 *				95    Bright Magenta	105	Bright Magenta
 *				96    Bright Cyan	106	Bright Cyan
 *				97    Bright White	107	Bright White
 */

#define __ansi_begin "\033["
#define __ansi_end "m"
#define __ansi_sep ";"

#define __ansi(...) CALL(__ansi_, __VA_ARGS__)
#define __ansi_1(x) __ansi_begin #x __ansi_end
#define __ansi_2(x, y) __ansi_begin #x __ansi_sep #y __ansi_end
#define __ansi_3(x, y, z) __ansi_begin #x __ansi_sep #y __ansi_sep #z __ansi_end

#define ANSI_RESET 0
#define ANSI_BOLD  1

#define ANSI_BLACK   30
#define ANSI_RED     31
#define ANSI_GREEN   32
#define ANSI_YELLOW  33
#define ANSI_BLUE    34
#define ANSI_MAGENTA 35
#define ANSI_CYAN    36
#define ANSI_WHITE   37

#define ANSI_BRIGHT_BLACK   90
#define ANSI_BRIGHT_RED     91
#define ANSI_BRIGHT_GREEN   92
#define ANSI_BRIGHT_YELLOW  93
#define ANSI_BRIGHT_BLUE    94
#define ANSI_BRIGHT_MAGENTA 95
#define ANSI_BRIGHT_CYAN    96
#define ANSI_BRIGHT_WHITE   97

#define ANSI_BG_BLACK   40
#define ANSI_BG_RED     41
#define ANSI_BG_GREEN   42
#define ANSI_BG_YELLOW  43
#define ANSI_BG_BLUE    44
#define ANSI_BG_MAGENTA 45
#define ANSI_BG_CYAN    46
#define ANSI_BG_WHITE   47

/**
 * macro H - Wrap a string literal with ANSI formatting and reset it afterward.
 * @x: String literal to format.
 * @...: One to three formatting codes.
 */
#define H(x, ...) __ansi(__VA_ARGS__) x __ansi(ANSI_RESET)

#endif /* ANSI_H */
