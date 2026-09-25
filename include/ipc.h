/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#ifndef IPC_H
#define IPC_H

#include <inttypes.h>

#define IPC_INTERFACE_NAME "sh.barroit." BUILD_REPO_NAME

enum ipc_request_type {
	IPC_REQ_FRAME,
	IPC_REQ_BRIGHTNESS,
	IPC_REQ_CLEAR,
	IPC_REQ_POWER,
	IPC_REQ_STATUS,
};

enum ipc_response_type {
	IPC_RES_SUCCESS,
	IPC_RES_ERROR,
	IPC_RES_STATUS,
};

struct ipc_device_status {
	;
};

struct ipc_request {
	enum ipc_request_type type;
	union {
		int64_t brightness;
	};
};

struct ipc_response {
	enum ipc_response_type type;
	union {
		const char *error;
		struct ipc_device_status status;
	};
};

void ipc_init(void);

void ipc_listen(void);

#endif /* IPC_H */
