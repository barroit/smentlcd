/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#ifndef DEVICE_H
#define DEVICE_H

#include "list.h"

struct dev_ctx;
struct ipc_ctx;

void dev_init(struct dev_ctx **ctx);

void dev_assign_ipc_ctx(struct dev_ctx *ctx, struct ipc_ctx *ipc_ctx);

void dev_setup_pollfd(struct dev_ctx *ctx);

void dev_enable_hotplug(struct dev_ctx *ctx);

int dev_available(struct dev_ctx *ctx);

#endif /* DEVICE_H */
