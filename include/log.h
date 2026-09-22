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

struct message {
	int fd;
	char buf[CONFIG_LOG_MESSAGE_BUFFER_SIZE];
	size_t len;
};

typedef void (*log_vwritef_fn)(int fd, const char *prefix, const char *hint,
			       const char *fmt, va_list ap);

typedef void (*log_nb_terminate_fn)(void);

extern char *strerror(int errnum);
extern const char *libusb_strerror(int errcode);

#define record(fmt, ...) __log_record(__func__, fmt, ##__VA_ARGS__)

#define warn(fmt, ...) __log_warn(NULL, fmt, ##__VA_ARGS__)
#define warn_errno(fmt, ...) warn_errno2(errno, fmt, ##__VA_ARGS__)
#define warn_errno2(e, fmt, ...) __log_warn(strerror(e), fmt, ##__VA_ARGS__)
#define warn_libusb(e, fmt, ...) \
	__log_warn(libusb_strerror(e), fmt, ##__VA_ARGS__)

#define error(fmt, ...) __log_error(NULL, fmt, ##__VA_ARGS__)
#define error_errno(fmt, ...) error_errno2(errno, fmt, ##__VA_ARGS__)
#define error_errno2(e, fmt, ...) __log_error(strerror(e), fmt, ##__VA_ARGS__)
#define error_libusb(e, fmt, ...) \
	__log_error(libusb_strerror(e), fmt, ##__VA_ARGS__)

#define die(fmt, ...) __log_die(NULL, fmt, ##__VA_ARGS__)
#define die_errno(fmt, ...) die_errno2(errno, fmt, ##__VA_ARGS__)
#define die_errno2(e, fmt, ...) __log_die(strerror(e), fmt, ##__VA_ARGS__)
#define die_libusb(e, fmt, ...) \
	__log_die(libusb_strerror(e), fmt, ##__VA_ARGS__)

#define bug(fmt, ...)							\
	__log_bug("%s:%d,%s(): " fmt, __FILE__, __LINE__, __func__,	\
		  ##__VA_ARGS__)

void __log_record(const char *func, const char *fmt, ...) __printf(2, 3);

int __log_warn(const char *hint, const char *fmt, ...) __printf(2, 3);

int __log_error(const char *hint, const char *fmt, ...) __printf(2, 3);

void __log_die(const char *hint, const char *fmt, ...) __printf(2, 3)
						       __noreturn;

void __log_bug(const char *fmt, ...) __printf(1, 2) __noreturn;

void __log_vwritef(int fd, const char *prefix, const char *hint,
		   const char *fmt, va_list ap);

extern log_vwritef_fn log_vwritef;

void log_writef(int fd, const char *prefix, const char *hint,
		const char *fmt, ...) __printf(4, 5);

void log_vprintf(FILE *stream, const char *prefix, const char *hint,
		 const char *fmt, va_list ap);

void log_printf(FILE *stream, const char *prefix, const char *hint,
		const char *fmt, ...) __printf(4, 5);

size_t __log_format_line(char *buf, size_t cap, const char *prefix,
			 const char *hint, const char *fmt, va_list ap);

extern int log_nb_enabled;

int __log_nb_init(void);

int log_nb_init(void);

extern log_nb_terminate_fn log_nb_terminate;

int __log_nb_ring_produce(int fd, const char *prefix, const char *hint,
			  const char *fmt, va_list ap);

int __log_nb_ring_consume(struct message *message);

void *__log_nb_worker(void *userdata);

void log_nb_activate(void);

void log_nb_deactivate(void);

void log_nb_auto_commit(int enabled);

void log_nb_wake_up(void);

#endif /* LOG_H */
