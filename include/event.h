/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#ifndef EVENT_H
#define EVENT_H

#include <stddef.h>

struct sd_event;

typedef void (*ev_handler_fn)(void);

extern struct sd_event *ev_current;

void ev_init(void);

void ev_start_loop(void);

int ev_sched_once(ev_handler_fn handler);

void *ev_watch_pollfd(size_t nalloc, int fd, short events);

void ev_unwatch_pollfd(void *src);

#endif /* EVENT_H */
