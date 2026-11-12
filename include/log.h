/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#ifndef LOG_H
#define LOG_H

#include <errno.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

#include "attr.h"

/**
 * typedef log_vwritef_fn - Message writer function type.
 * @fd: Output file descriptor.
 * @prefix: Optional message prefix.
 * @hint: Optional message hint.
 * @fmt: printf-style format string.
 * @ap: Argument list.
 */
typedef void (*log_vwritef_fn)(int fd, const char *prefix, const char *hint,
			       const char *fmt, va_list ap);

extern char *strerror(int errnum);
extern const char *libusb_strerror(int errcode);
extern const char *stbi_failure_reason(void);

/*
 * var log_vwritef - Output handler for log_writef() and logging macros.
 *
 * Defaults to __log_vwritef(). Assign another handler to customize output.
 */
extern log_vwritef_fn log_vwritef;

/**
 * macro record - Write a message to stdout prefixed with the function name.
 * @fmt: printf-style format string.
 * @...: Format arguments.
 *
 * Include a CLOCK_MONOTONIC timestamp when CONFIG_RECORD_SHOW_TIMESTAMP
 * is enabled.
 */
#define record(fmt, ...) __log_record(__func__, fmt, ##__VA_ARGS__)

/**
 * macro warn - Write a warning to stderr.
 * @fmt: printf-style format string.
 * @...: Format arguments.
 */
#define warn(fmt, ...) __log_warn(NULL, fmt, ##__VA_ARGS__)

/**
 * macro warn_errno - Write a warning with the current errno description.
 * @fmt: printf-style format string.
 * @...: Format arguments.
 */
#define warn_errno(fmt, ...) warn_errno2(errno, fmt, ##__VA_ARGS__)

/**
 * macro warn_errno2 - Write a warning with the description of @e.
 * @e: Error number.
 * @fmt: printf-style format string.
 * @...: Format arguments.
 */
#define warn_errno2(e, fmt, ...) __log_warn(strerror(e), fmt, ##__VA_ARGS__)

/**
 * macro warn_libusb - Write a warning with the libusb error description.
 * @e: libusb error number.
 * @fmt: printf-style format string.
 * @...: Format arguments.
 */
#define warn_libusb(e, fmt, ...) \
	__log_warn(libusb_strerror(e), fmt, ##__VA_ARGS__)

/**
 * macro error - Write an error message to stderr.
 * @fmt: printf-style format string.
 * @...: Format arguments.
 */
#define error(fmt, ...) __log_error(NULL, fmt, ##__VA_ARGS__)

/**
 * macro error_errno - Write an error with the current errno description.
 * @fmt: printf-style format string.
 * @...: Format arguments.
 */
#define error_errno(fmt, ...) error_errno2(errno, fmt, ##__VA_ARGS__)

/**
 * macro error_errno2 - Write an error with the description of @e.
 * @e: Error number.
 * @fmt: printf-style format string.
 * @...: Format arguments.
 */
#define error_errno2(e, fmt, ...) __log_error(strerror(e), fmt, ##__VA_ARGS__)

/**
 * macro error_libusb - Write an error with the libusb error description.
 * @e: libusb error number.
 * @fmt: printf-style format string.
 * @...: Format arguments.
 */
#define error_libusb(e, fmt, ...) \
	__log_error(libusb_strerror(e), fmt, ##__VA_ARGS__)

/**
 * macro die - Write a fatal error to stderr and exit with status 128.
 * @fmt: printf-style format string.
 * @...: Format arguments.
 */
#define die(fmt, ...) __log_die(NULL, fmt, ##__VA_ARGS__)

/**
 * macro die_errno - Write a fatal error with the current errno description and
 *		     exit with status 128.
 * @fmt: printf-style format string.
 * @...: Format arguments.
 */
#define die_errno(fmt, ...) die_errno2(errno, fmt, ##__VA_ARGS__)

/**
 * macro die_errno2 - Write a fatal error with the description of @e and exit
 *		      with status 128.
 * @e: Error number.
 * @fmt: printf-style format string.
 * @...: Format arguments.
 */
#define die_errno2(e, fmt, ...) __log_die(strerror(e), fmt, ##__VA_ARGS__)

/**
 * macro die_libusb - Write a fatal error with the libusb error description and
 *		      exit with status 128.
 * @e: libusb error number.
 * @fmt: printf-style format string.
 * @...: Format arguments.
 */
#define die_libusb(e, fmt, ...) \
	__log_die(libusb_strerror(e), fmt, ##__VA_ARGS__)

/**
 * macro die_stbi - Write a fatal error with the stb_image failure reason and
 *		    exit with status 128.
 * @fmt: printf-style format string.
 * @...: Format arguments.
 */
#define die_stbi(fmt, ...) __log_die(stbi_failure_reason(), fmt, ##__VA_ARGS__)

/**
 * macro bug - Write a diagnostic to stderr with the source file, line number,
 *	       and function name, then abort.
 * @fmt: printf-style format string.
 * @...: Format arguments.
 */
#define bug(fmt, ...)							\
	__log_bug("%s:%d,%s(): " fmt, __FILE__, __LINE__, __func__,	\
		  ##__VA_ARGS__)

void __log_record(const char *func, const char *fmt, ...) attr_printf(2, 3);

void __log_warn(const char *hint, const char *fmt, ...) attr_printf(2, 3);

void __log_error(const char *hint, const char *fmt, ...) attr_printf(2, 3);

void __log_die(const char *hint, const char *fmt, ...) attr_printf(2, 3)
						       attr_noreturn;

void __log_bug(const char *fmt, ...) attr_printf(1, 2) attr_noreturn;

void __log_vwritef(int fd, const char *prefix, const char *hint,
		   const char *fmt, va_list ap);

/**
 * log_writef() - Write a formatted message to a file descriptor.
 * @fd: Output file descriptor.
 * @prefix: Optional message prefix.
 * @hint: Optional message hint.
 * @fmt: printf-style format string.
 * @...: Format arguments.
 */
void log_writef(int fd, const char *prefix, const char *hint,
		const char *fmt, ...) attr_printf(4, 5);

/**
 * log_vprintf() - Write a formatted message to a stream using a va_list.
 * @stream: Output stream.
 * @prefix: Optional message prefix.
 * @hint: Optional message hint.
 * @fmt: printf-style format string.
 * @ap: Argument list.
 */
void log_vprintf(FILE *stream, const char *prefix, const char *hint,
		 const char *fmt, va_list ap);

/**
 * log_printf() - Write a formatted message to a stream.
 * @stream: Output stream.
 * @prefix: Optional message prefix.
 * @hint: Optional message hint.
 * @fmt: printf-style format string.
 * @...: Format arguments.
 */
void log_printf(FILE *stream, const char *prefix, const char *hint,
		const char *fmt, ...) attr_printf(4, 5);

size_t __log_format_line(char *buf, size_t cap, const char *prefix,
			 const char *hint, const char *fmt, va_list ap);

#endif /* LOG_H */
