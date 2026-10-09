/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#ifndef EVENT_H
#define EVENT_H

#include <stddef.h>

typedef void (*event_handler_fn)(void);

void event_init(void);

void *event_current(void);

void event_start_loop(void);

int event_sched_once(event_handler_fn handler);

void *event_watch_pollfd(size_t nalloc, int fd, short events);

void event_unwatch_pollfd(void *src);

#endif /* EVENT_H */
