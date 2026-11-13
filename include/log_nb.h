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

extern log_nb_terminate_fn log_nb_terminate;

/**
 * log_nb_catch_signal() - Block SIGTERM and start a signal waiter thread.
 *
 * Return: Zero on success, or -1 if thread creation fails.
 */
int log_nb_catch_signal(void);

/**
 * log_nb_restore_signal() - Cancel and join the signal waiter without
 *			     restoring the previous signal mask.
 */
void log_nb_restore_signal(void);

int __log_nb_init(void);

/**
 * log_nb_init() - Initialize the non-blocking logging backend.
 *
 * Return: Zero on success, or -1 on failure.
 */
int log_nb_init(void);

int __log_nb_ring_produce(int fd, const char *prefix, const char *hint,
			  const char *fmt, va_list ap);

int __log_nb_ring_consume(struct message *message);

void *__log_nb_worker(void *userdata);

/**
 * log_nb_activate() - Enable non-blocking logging and its termination handler.
 */
void log_nb_activate(void);

/**
 * log_nb_deactivate() - Restore the default message writer without stopping
 *			 the worker thread.
 */
void log_nb_deactivate(void);

/**
 * log_nb_auto_commit() - Enable or disable automatic worker notifications.
 * @enabled: Nonzero to enable, zero to disable.
 */
void log_nb_auto_commit(int enabled);

/**
 * log_nb_wake_up() - Notify the logging worker to process pending messages.
 */
void log_nb_wake_up(void);

#endif /* LOG_NB_H */
