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

#include "log.h"
#include "xalloc.h"

struct ev_ctx {
	struct sd_event *event;
};

int dev_wake_libusb(void);

static struct ev_ctx ctx;

struct sd_event *ev_current;

void ev_init(void)
{
	int err;

	assert(!ctx.event);

	err = sd_event_default(&ctx.event);
	if (err < 0)
		die_errno2(-err, "sd_event_default() failed");

	ev_current = ctx.event;
}

void ev_start_loop(void)
{
	int ret;

	ret = sd_event_loop(ctx.event);
	if (ret < 0)
		die_errno2(-ret, "sd_event_loop() failed");
}

static int handle_event_io(sd_event_source *src, int fd, uint32_t revents,
			   void *userdata)
{
	return dev_wake_libusb();
}

void *ev_watch_pollfd(size_t nalloc, int fd, short events)
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

void ev_unwatch_pollfd(void *src)
{
	sd_event_source_unref(*(struct sd_event_source **)src);
}
