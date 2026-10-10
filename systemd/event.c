// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#include "event.h"

#include <assert.h>
#include <poll.h>
#include <stddef.h>
#include <stdlib.h>
#include <systemd/sd-event.h>

#include "list.h"
#include "log.h"
#include "xalloc.h"
#include "size.h"

struct once_queue {
	event_once_fn handler[SZ_16];
	size_t head;
	size_t tail;
};

struct timeout_table {
	sd_event_source *source[SZ_16];
	event_timeout_fn handler[SZ_16];
};

struct event_ctx {
	struct sd_event *event;

	struct once_queue once_start;
	struct timeout_table timeout_start;
};

int dev_wake_libusb(void);

static struct event_ctx ctx;

void event_init(void)
{
	int err;

	err = sd_event_default(&ctx.event);
	if (err < 0)
		die_errno2(-err, "sd_event_default() failed");
}

void *event_current(void)
{
	return ctx.event;
}

void event_start_loop(void)
{
	int ret;

	ret = sd_event_loop(ctx.event);
	if (ret < 0)
		die_errno2(-ret, "sd_event_loop() failed");
}

static int handle_once(sd_event_source *src, void *userdata)
{
	size_t tail;

	tail = ctx.once_start.tail % SZ_16;
	ctx.once_start.tail++;
	ctx.once_start.handler[tail]();

	sd_event_source_unref(src);
	return 0;
}

int event_sched_once(event_once_fn handler)
{
	int err;
	size_t head;
	struct sd_event_source *src;

	assert(ctx.once_start.head - ctx.once_start.tail != SZ_16);

	head = ctx.once_start.head % SZ_16;
	ctx.once_start.head++;
	ctx.once_start.handler[head] = handler;
	err = sd_event_add_defer(ctx.event, &src, handle_once, NULL);
	if (err < 0) {
		ctx.once_start.head--;
		error_errno2(-err, "can't schedule event");
		return -1;
	}

	return 0;
}

static int handle_timeout(sd_event_source *src, uint64_t usec, void *userdata)
{
	sd_event_source **slot;
	unsigned int idx;

	slot = userdata;
	idx = slot - ctx.timeout_start.source;

	*slot = NULL;
	return ctx.timeout_start.handler[idx]();
}

int event_sched_timeout(void **timer, uint64_t msec, event_timeout_fn handler)
{
	int err;
	unsigned int idx;

	for (idx = 0; idx < sizeof_array(ctx.timeout_start.source); idx++)
		if (!ctx.timeout_start.source[idx])
			break;
	assert(idx != sizeof_array(ctx.timeout_start.source));

	err = sd_event_add_time_relative(ctx.event,
					 &ctx.timeout_start.source[idx],
					 CLOCK_MONOTONIC, msec * 1000, 1,
					 handle_timeout,
					 &ctx.timeout_start.source[idx]);
	if (err < 0) {
		error_errno2(-err, "can't schedule event");
		return -1;
	}

	ctx.timeout_start.handler[idx] = handler;
	*timer = ctx.timeout_start.source[idx];
	return 0;
}

int event_resched_timeout(void *timer, uint64_t msec)
{
	int err;
	uint64_t prev;

	err = sd_event_source_get_time(timer, &prev);
	if (err < 0) {
		error_errno2(-err, "can't retrieve previous delay from timer");
		return -1;
	}

	err = sd_event_source_set_time(timer, prev + msec * 1000);
	if (err < 0) {
		error_errno2(-err, "can't reschedule timer");
		return -1;
	}

	err = sd_event_source_set_enabled(timer, SD_EVENT_ONESHOT);
	if (err < 0) {
		error_errno2(-err, "can't enable timer for next timeout");
		return -1;
	}

	return 0;
}

void event_destroy_timeout(void *timer)
{
	unsigned int idx;

	for (idx = 0; idx < sizeof_array(ctx.timeout_start.source); idx++)
		if (ctx.timeout_start.source[idx] == timer) {
			ctx.timeout_start.source[idx] = NULL;
			break;
		}

	sd_event_source_disable_unref(timer);
}

static int handle_event_io(sd_event_source *src, int fd, uint32_t revents,
			   void *userdata)
{
	return dev_wake_libusb();
}

void *event_watch_pollfd(size_t nalloc, int fd, short events)
{
	int err;
	char *buf;
	struct sd_event_source **src;
	uint32_t sd_events = 0;

	if (events & POLLIN)
		sd_events |= EPOLLIN;

	if (events & POLLOUT)
		sd_events |= EPOLLOUT;

	buf = xmalloc(nalloc + sizeof(*src));
	src = (typeof(src))&buf[nalloc];

	err = sd_event_add_io(ctx.event, src, fd, sd_events, handle_event_io,
			      NULL);
	if (err) {
		error_errno2(-err,
			     "unable to add libusb pollfd %d as new I/O event source to event loop",
			     fd);
		free(buf);
		return NULL;
	}

	return buf;
}

void event_unwatch_pollfd(void *src)
{
	sd_event_source_unref(*(struct sd_event_source **)src);
}
