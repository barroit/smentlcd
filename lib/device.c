// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#include "device.h"

#include <stdlib.h>

#include "ipc.h"
#include "libusb.h"
#include "log.h"

#define hotplug_register libusb_hotplug_register_callback

struct dev_ctx {
	struct list_head ev_src_list;

	struct libusb_device *dev;
	struct libusb_device_handle *dh;
	struct libusb_device_descriptor dd;
};

static struct dev_ctx ctx;

void dev_init(void)
{
	int err;

	list_head_init(&ctx.ev_src_list);

	err = libusb_init_context(NULL, NULL, 0);
	if (err < 0)
		die_libusb(err, "unable to init libusb context");

	if (!libusb_pollfds_handle_timeouts(NULL))
		die("your platform doesn't support automatically waking up when a USB transfer timeout expires");
}

static void watch_pollfd(int fd, short events, void *userdata)
{
	struct event_source *ev_src;

	ev_src = ipc_watch_pollfd(sizeof(*ev_src), fd, events);
	if (!ev_src)
		return;

	ev_src->fd = fd;
	list_add_tail(&ev_src->list, &ctx.ev_src_list);
}

static void unwatch_pollfd(int fd, void *userdata)
{
	struct event_source *ev_src;

	list_foreach_entry(ev_src, &ctx.ev_src_list, list) {
		if (ev_src->fd == fd) {
			ipc_unwatch_pollfd(ev_src->data);
			list_del(&ev_src->list);

			free(ev_src);
			break;
		}
	}
}

void dev_setup_pollfd(void)
{
	const struct libusb_pollfd **__fds;
	const struct libusb_pollfd **fds;

	__fds = libusb_get_pollfds(NULL);
	if (!__fds)
		die("can't retrieve libusb pollfds");

	for (fds = __fds; *fds; fds++)
		watch_pollfd((*fds)->fd, (*fds)->events, NULL);

	libusb_set_pollfd_notifiers(NULL, watch_pollfd, unwatch_pollfd, NULL);

	libusb_free_pollfds(__fds);
}

static int handle_hotplug(struct libusb_context *libusb,
			  struct libusb_device *dev,
			  libusb_hotplug_event event, void *userdata)
{
	int err;

	switch (event) {
	case LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED:
		err = libusb_open(dev, &ctx.dh);
		if (err) {
			error_libusb(err, "can't open device for I/O");
			return 0;
		}

		ctx.dev = dev;
		/*
		 * Since libusb-1.0.16, this function always succeeds.
		 */
		libusb_get_device_descriptor(dev, &ctx.dd);

		record("device %" PRIx16 ":%" PRIx16 " plugged",
		       ctx.dd.idVendor, ctx.dd.idProduct);
		break;

	case LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT:
		libusb_close(ctx.dh);
		ctx.dev = NULL;
		ctx.dh = NULL;

		record("device %" PRIx16 ":%" PRIx16 " unplugged",
		       ctx.dd.idVendor, ctx.dd.idProduct);
	}

	return 0;
}

void dev_enable_hotplug(void)
{
	int err;

	err = hotplug_register(NULL,
			       LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED |
			       LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT,
			       LIBUSB_HOTPLUG_ENUMERATE,
			       CONFIG_SMENT_VID, CONFIG_SMENT_PID,
			       LIBUSB_HOTPLUG_MATCH_ANY,
			       handle_hotplug, NULL, NULL);
	if (err)
		die_libusb(err, "failed to register hotplug event callback");
}

int dev_available(void)
{
	return !!ctx.dev;
}
