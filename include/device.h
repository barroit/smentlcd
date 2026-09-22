/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#ifndef DEVICE_H
#define DEVICE_H

#include <stdalign.h>
#include <stddef.h>

#include "list.h"

struct event_source {
	int fd;
	struct list_head list;

	alignas(max_align_t) char data[];
};

void dev_init(void);

void dev_setup_pollfd(void);

void dev_enable_hotplug(void);

int dev_available(void);

#endif /* DEVICE_H */
