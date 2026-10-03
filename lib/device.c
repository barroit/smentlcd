// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#include "device.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "event.h"
#include "libusb.h"
#include "log.h"
#include "size.h"

#define hotplug_register libusb_hotplug_register_callback

#define DEV_PLUGGED (1 << 0)
#define DEV_ENABLED (1 << 1)

struct dev_ctx {
	uint32_t status;
	struct list_head ev_src_list;

	struct libusb_device *dev;
	struct libusb_device_handle *dh;
	struct libusb_device_descriptor dd;
};

int dev_wake_libusb(void);

static struct dev_ctx ctx;

int dev_wake_libusb(void)
{
	int err;
	struct timeval tv = { 0 };

	err = libusb_handle_events_timeout(NULL, &tv);
	if (err < 0) {
		error_libusb(err, "libusb can't handle pending events");
		return -1;
	}

	return 0;
}

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
	size_t nalloc = cc_offsetof(struct event_source, data);

	ev_src = ev_watch_pollfd(nalloc, fd, events);
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
			ev_unwatch_pollfd(ev_src->data);
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

static void disable_device(void)
{
	ctx.status &= ~DEV_ENABLED;
	record("device disabled");
}

static void claim_lcd_interface(void)
{
	int err;

	if (!(ctx.status & DEV_PLUGGED))
		return;

	err = libusb_claim_interface(ctx.dh, 0);
	if (err) {
		error_libusb(err, "can't claim device interface for LCD I/O");
		disable_device();
	} else {
		ctx.status |= DEV_ENABLED;
	}
}

static int handle_hotplug(struct libusb_context *libusb,
			  struct libusb_device *dev,
			  libusb_hotplug_event event, void *userdata)
{
	int err;

	switch (event) {
	case LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED:
		ctx.status |= DEV_PLUGGED;

		ctx.dev = dev;
		/*
		 * Since libusb-1.0.16, this function always succeeds.
		 */
		libusb_get_device_descriptor(dev, &ctx.dd);

		record("device %" PRIx16 ":%" PRIx16 " plugged",
		       ctx.dd.idVendor, ctx.dd.idProduct);

		err = libusb_open(dev, &ctx.dh);
		if (err) {
			error_libusb(err, "can't open device for I/O");
			return 0;
		}

		err = ev_sched_once(claim_lcd_interface);
		if (err) {
			disable_device();
			break;
		}

		break;

	case LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT:
		ctx.status = 0;

		record("device %" PRIx16 ":%" PRIx16 " unplugged",
		       ctx.dd.idVendor, ctx.dd.idProduct);

		libusb_close(ctx.dh);
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

int dev_enabled(void)
{
	return ctx.status & DEV_ENABLED;
}

int dev_get_vendor_id(const char **ret)
{
	static char buf[SZ_16];

	snprintf(buf, sizeof(buf), "0x%04" PRIx16, ctx.dd.idVendor);
	*ret = buf;
	return 0;
}

int dev_get_manufacturer(const char **ret)
{
	int err;
	static char buf[LIBUSB_DEVICE_STRING_BYTES_MAX];

	err = libusb_get_device_string(ctx.dev, LIBUSB_DEVICE_STRING_MANUFACTURER,
				       buf, sizeof(buf));
	if (err < 0) {
		warn_libusb(err, "failed to query manufacturer name");
		return 1;
	}

	*ret = buf;
	return 0;
}

int dev_get_product_id(const char **ret)
{
	static char buf[SZ_16];

	snprintf(buf, sizeof(buf), "0x%04" PRIx16, ctx.dd.idProduct);
	*ret = buf;
	return 0;
}

int dev_get_product(const char **ret)
{
	int err;
	static char buf[LIBUSB_DEVICE_STRING_BYTES_MAX];

	err = libusb_get_device_string(ctx.dev, LIBUSB_DEVICE_STRING_PRODUCT,
				       buf, sizeof(buf));
	if (err < 0) {
		warn_libusb(err, "failed to query product name");
		return 1;
	}

	*ret = buf;
	return 0;
}

int dev_get_device_version(const char **ret)
{
	static char buf[SZ_16];

	snprintf(buf, sizeof(buf), "%" PRIx16 ".%" PRIx16,
		 ctx.dd.bcdDevice >> 8, ctx.dd.bcdDevice & 0xff);

	*ret = buf;
	return 0;
}

int dev_get_serial_number(const char **ret)
{
	int err;
	static char buf[LIBUSB_DEVICE_STRING_BYTES_MAX];

	err = libusb_get_device_string(ctx.dev, LIBUSB_DEVICE_STRING_SERIAL_NUMBER,
				       buf, sizeof(buf));
	if (err < 0) {
		warn_libusb(err, "failed to query serial number");
		return 1;
	}

	*ret = buf;
	return 0;
}

int dev_get_usb_version(const char **ret)
{
	static char buf[SZ_16];

	snprintf(buf, sizeof(buf), "%" PRIx16 ".%" PRIx16, ctx.dd.bcdUSB >> 8,
		 ctx.dd.bcdUSB & 0xff);

	*ret = buf;
	return 0;
}

int dev_get_bus_number(uint8_t *ret)
{
	*ret = libusb_get_bus_number(ctx.dev);
	return 0;
}

int dev_get_device_address(uint8_t *ret)
{
	*ret = libusb_get_device_address(ctx.dev);
	return 0;
}

int dev_get_port(uint8_t **ret, size_t *len)
{
	int err;
	static uint8_t path[SZ_16];

	err = libusb_get_port_numbers(ctx.dev, path, sizeof(path));
	if (err < 0) {
		warn_libusb(err, "failed to query port path");
		return 1;
	}

	*ret = path;
	*len = err;
	return 0;
}

int dev_get_speed(const char **ret)
{
	switch (libusb_get_device_speed(ctx.dev)) {
	case LIBUSB_SPEED_LOW:
		*ret = "1.5 Mbit/s";
		break;
	case LIBUSB_SPEED_FULL:
		*ret = "12 Mbit/s";
		break;
	case LIBUSB_SPEED_HIGH:
		*ret = "480 Mbit/s";
		break;
	case LIBUSB_SPEED_SUPER:
		*ret = "5000 Mbit/s";
		break;
	case LIBUSB_SPEED_SUPER_PLUS:
		*ret = "10000 Mbit/s";
		break;
	case LIBUSB_SPEED_SUPER_PLUS_X2:
		*ret = "20000 Mbit/s";
		break;
	default:
		*ret = "unknown";
		break;
	}

	return 0;
}
