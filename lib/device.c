// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#include "device.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#include "calc.h"
#include "event.h"
#include "libusb.h"
#include "log.h"
#include "playback.h"
#include "size.h"

#define libusb_hotplug_register libusb_hotplug_register_callback

#define DEV_PLUGGED (1 << 0)
#define DEV_ENABLED (1 << 1)

struct transfer_buffer {
	uint8_t (*buf)[sizeof(((struct frame *)0)->buf)];
	uint32_t used;
};

struct dev_ctx {
	uint32_t status;
	struct list_head events;

	struct libusb_device *dev;
	struct libusb_device_handle *dh;
	struct libusb_device_descriptor dd;

	struct transfer_buffer tb;
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

	list_head_init(&ctx.events);

	err = libusb_init_context(NULL, NULL, 0);
	if (err < 0)
		die_libusb(err, "unable to init libusb context");

	if (!libusb_pollfds_handle_timeouts(NULL))
		die("your platform doesn't support automatically waking up when a USB transfer timeout expires");

	ctx.tb.buf = mmap(NULL, bitsof(ctx.tb.used) * sizeof(*ctx.tb.buf),
			  PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS,
			  -1, 0);
	if (ctx.tb.buf == MAP_FAILED)
		die_errno("can't init transfer buffer");
}

static void watch_pollfd(int fd, short events, void *userdata)
{
	struct event_source *event;
	size_t nalloc = cc_offsetof(struct event_source, data);

	event = event_watch_pollfd(nalloc, fd, events);
	if (!event)
		return;

	event->fd = fd;
	list_add_tail(&event->list, &ctx.events);
}

static void unwatch_pollfd(int fd, void *userdata)
{
	struct event_source *event;

	list_foreach_entry(event, &ctx.events, list) {
		if (event->fd == fd) {
			event_unwatch_pollfd(event->data);
			list_del(&event->list);

			free(event);
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
		if (ctx.status & DEV_PLUGGED)
			break;

		ctx.status |= DEV_PLUGGED;

		ctx.dev = dev;
		libusb_get_device_descriptor(dev, &ctx.dd);

		record("device %" PRIx16 ":%" PRIx16 " plugged",
		       ctx.dd.idVendor, ctx.dd.idProduct);

		err = libusb_open(dev, &ctx.dh);
		if (err) {
			error_libusb(err, "can't open device for I/O");
			return 0;
		}

		err = event_sched_once(claim_lcd_interface);
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

	err = libusb_hotplug_register(NULL,
				      LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED |
				      LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT,
				      LIBUSB_HOTPLUG_ENUMERATE,
				      CONFIG_DEVICE_VID, CONFIG_DEVICE_PID,
				      LIBUSB_HOTPLUG_MATCH_ANY,
				      handle_hotplug, NULL, NULL);
	if (err)
		die_libusb(err, "failed to register hotplug event callback");
}

int dev_enabled(void)
{
	return ctx.status & DEV_ENABLED;
}

static void handle_transfer_done(struct libusb_transfer *transfer)
{
	typeof(ctx.tb.buf) slot;
	unsigned int idx;

	switch (transfer->status) {
	case LIBUSB_TRANSFER_NO_DEVICE:
		error("device disconnected during frame transfer");
		disable_device();
		break;
	case LIBUSB_TRANSFER_TIMED_OUT:
		warn("frame transfer timed out");
		break;
	case LIBUSB_TRANSFER_STALL:
		error("device state corrupted");
		disable_device();
		break;
	}

	slot = transfer->user_data;
	idx = slot - ctx.tb.buf;

	ctx.tb.used &= ~(UINT32_C(1) << idx);
	libusb_free_transfer(transfer);
}

static unsigned int acquire_transfer_slot(void)
{
	unsigned int idx;

	for (idx = 0; idx < bitsof(ctx.tb.used); idx++) {
		uint32_t mask;

		mask = UINT32_C(1) << idx;
		if (!(ctx.tb.used & mask)) {
			ctx.tb.used |= mask;
			return idx;
		}
	}

	return -1;
}

int dev_submit_frame(uint8_t *buf, unsigned int size, unsigned int idx)
{
	int err;
	unsigned int slot;
	struct libusb_transfer *transfer;

	transfer = libusb_alloc_transfer(0);
	if (!transfer) {
		error_libusb(LIBUSB_ERROR_NO_MEM,
			     "can't allocate USB transfer");
		return -1;
	}

	slot = acquire_transfer_slot();
	if (slot == -1) {
		warn("frame %u dropped due to transfer buffer slot exhaustion",
		     idx);
		libusb_free_transfer(transfer);
		return 0;
	}

	memcpy(ctx.tb.buf[slot], buf, size);
	libusb_fill_bulk_transfer(transfer, ctx.dh, CONFIG_DEVICE_LCD_ENDPOINT,
				  ctx.tb.buf[slot], size, handle_transfer_done,
				  &ctx.tb.buf[slot],
				  CONFIG_DEVICE_TRANSFER_TIMEOUT);

	err = libusb_submit_transfer(transfer);
	if (err) {
		error_libusb(err, "can't submit transfer");
		ctx.tb.used &= ~(UINT32_C(1) << slot);
		libusb_free_transfer(transfer);
		return -1;
	}

	return 0;
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

int dev_get_port(uint8_t **ret, uint8_t *len)
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
