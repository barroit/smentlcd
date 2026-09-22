/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#ifndef DEVICE_H
#define DEVICE_H

void dev_init(void);

void dev_setup_pollfd(void);

void dev_enable_hotplug(void);

int dev_available(void);

#endif /* DEVICE_H */
