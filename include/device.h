/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#ifndef DEVICE_H
#define DEVICE_H

#include <stdalign.h>
#include <stddef.h>
#include <stdint.h>

#include "list.h"

struct event_source {
	int fd;
	struct list_head list;

	alignas(max_align_t) char data[];
};

void dev_init(void);

void dev_setup_pollfd(void);

void dev_enable_hotplug(void);

int dev_enabled(void);

int dev_get_vendor_id(const char **ret);

int dev_get_manufacturer(const char **ret);

int dev_get_product_id(const char **ret);

int dev_get_product(const char **ret);

int dev_get_device_version(const char **ret);

int dev_get_serial_number(const char **ret);

int dev_get_usb_version(const char **ret);

int dev_get_bus_number(uint8_t *ret);

int dev_get_device_address(uint8_t *ret);

int dev_get_port(uint8_t **ret, size_t *len);

int dev_get_speed(const char **ret);

#endif /* DEVICE_H */
