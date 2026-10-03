/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#ifndef IPC_H
#define IPC_H

#include <stddef.h>
#include <stdint.h>

#define IPC_INTERFACE_NAME "sh.barroit." BUILD_REPO_NAME

#define IPC_DEV_STAT_VENDOR_ID		(1u << 0)
#define IPC_DEV_STAT_MANUFACTURER	(1u << 1)
#define IPC_DEV_STAT_PRODUCT_ID		(1u << 2)
#define IPC_DEV_STAT_PRODUCT		(1u << 3)
#define IPC_DEV_STAT_DEVICE_VERSION	(1u << 4)
#define IPC_DEV_STAT_SERIAL_NUMBER	(1u << 5)
#define IPC_DEV_STAT_USB_VERSION	(1u << 6)
#define IPC_DEV_STAT_BUS_NUMBER		(1u << 7)
#define IPC_DEV_STAT_DEVICE_ADDRESS	(1u << 8)
#define IPC_DEV_STAT_PORT		(1u << 9)
#define IPC_DEV_STAT_SPEED		(1u << 10)

struct ipc_dev_stat {
	uint32_t field;

	const char *vendor_id;
	const char *manufacturer;

	const char *product_id;
	const char *product;

	const char *device_version;
	const char *serial_number;
	const char *usb_version;

	uint8_t bus_number;
	uint8_t device_address;
	uint8_t *port;
	size_t port_count;
	const char *speed;
};

enum ipc_request_type {
	IPC_REQ_FRAME,
	IPC_REQ_CLEAR,
	IPC_REQ_STAT,
};

enum ipc_response_type {
	IPC_RES_SUCCESS,
	IPC_RES_ERROR,
	IPC_RES_STAT,
};

struct ipc_request {
	enum ipc_request_type type;
	union {
	};
};

struct ipc_response {
	enum ipc_response_type type;
	union {
		const char *error;
		struct ipc_dev_stat stat;
	};
};

void ipc_init_d(void);

void ipc_listen_d(void);

#endif /* IPC_H */
