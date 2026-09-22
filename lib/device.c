// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#include "device.h"

#include "ipc.h"
#include "libusb.h"
#include "log.h"

#include <stdalign.h>
#include <stdlib.h>

#define hotplug_register libusb_hotplug_register_callback

struct event_source {
	int fd;
	struct list_head list;

	alignas(max_align_t) char data[];
};

struct dev_ctx {
	struct ipc_ctx *ipc_ctx;
	struct list_head ev_src_list;

	struct libusb_device *dev;
	struct libusb_device_descriptor dd;
};

void dev_init(struct dev_ctx **__ctx)
{
	int err;
	static struct dev_ctx ctx;

	list_head_init(&ctx.ev_src_list);

	err = libusb_init_context(NULL, NULL, 0);
	if (err < 0)
		die_libusb(err, "unable to init libusb context");

	if (!libusb_pollfds_handle_timeouts(NULL))
		die("your platform doesn't support automatically waking up when a USB transfer timeout expires");

	*__ctx = &ctx;
}

void dev_assign_ipc_ctx(struct dev_ctx *ctx, struct ipc_ctx *ipc_ctx)
{
	ctx->ipc_ctx = ipc_ctx;
}

static void watch_pollfd(int fd, short events, void *userdata)
{
	struct dev_ctx *ctx = userdata;
	struct event_source *ev_src;

	ev_src = ipc_watch_pollfd(ctx->ipc_ctx, sizeof(*ev_src), fd, events);
	if (!ev_src)
		return;

	ev_src->fd = fd;
	list_add_tail(&ev_src->list, &ctx->ev_src_list);
}

static void unwatch_pollfd(int fd, void *userdata)
{
	struct dev_ctx *ctx = userdata;
	struct event_source *ev_src;

	list_foreach_entry(ev_src, &ctx->ev_src_list, list) {
		if (ev_src->fd == fd) {
			ipc_unwatch_pollfd(ctx->ipc_ctx, ev_src->data);
			list_del(&ev_src->list);

			free(ev_src);
			break;
		}
	}
}

void dev_setup_pollfd(struct dev_ctx *ctx)
{
	const struct libusb_pollfd **__fds;
	const struct libusb_pollfd **fds;

	__fds = libusb_get_pollfds(NULL);
	if (!__fds)
		die("can't retrieve libusb pollfds");

	for (fds = __fds; *fds; fds++)
		watch_pollfd((*fds)->fd, (*fds)->events, ctx);

	libusb_set_pollfd_notifiers(NULL, watch_pollfd, unwatch_pollfd, ctx);

	libusb_free_pollfds(__fds);
}

static int handle_hotplug(struct libusb_context *libusb,
			  struct libusb_device *dev,
			  libusb_hotplug_event event, void *userdata)
{
	struct dev_ctx *ctx = userdata;

	switch (event) {
	case LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED:
		ctx->dev = libusb_ref_device(dev);
		/*
		 * Since libusb-1.0.16, this function always succeeds.
		 */
		libusb_get_device_descriptor(dev, &ctx->dd);
		record("device %" PRIx16 ":%" PRIx16 " plugged",
		       ctx->dd.idVendor, ctx->dd.idProduct);
		break;

	case LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT:
		libusb_unref_device(ctx->dev);
		ctx->dev = NULL;
		record("device %" PRIx16 ":%" PRIx16 " unplugged",
		       ctx->dd.idVendor, ctx->dd.idProduct);
	}

	return 0;
}

void dev_enable_hotplug(struct dev_ctx *ctx)
{
	int err;

	err = hotplug_register(NULL, LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED |
				     LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT,
			       LIBUSB_HOTPLUG_ENUMERATE, CONFIG_SMENT_VID,
			       CONFIG_SMENT_PID, LIBUSB_HOTPLUG_MATCH_ANY,
			       handle_hotplug, ctx, NULL);
	if (err)
		die_libusb(err, "failed to register hotplug event callback");
}
