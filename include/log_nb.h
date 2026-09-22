/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#ifndef LOG_NB_H
#define LOG_NB_H

#include <stdarg.h>
#include <stddef.h>

#define LOG_NB_SYSTEM_NAME "non-blocking logging system backend"

struct message {
	int fd;
	char buf[CONFIG_LOG_MESSAGE_BUFFER_SIZE];
	size_t len;
};

typedef void (*log_nb_terminate_fn)(void);

extern int log_nb_enabled;

int log_nb_catch_signal(void);

void log_nb_restore_signal(void);

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

#endif /* LOG_NB_H */
