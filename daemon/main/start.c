// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#include "compiler.h"
#include "device.h"
#include "event.h"
#include "ipc.h"

void daemon_exec_req(struct ipc_request *req, struct ipc_response *res);

const char *cmd_main_start_help = "start the daemon process";

static void stat_device(struct ipc_response *res)
{
	struct ipc_dev_stat init = {
		.vendor_id = "",
		.manufacturer = "",
		.product_id = "",
		.product = "",
		.device_version = "",
		.serial_number = "",
		.usb_version = "",
		.speed = "",
	};

	res->type = IPC_RES_STAT;
	res->stat = init;

	if (!dev_get_vendor_id(&res->stat.vendor_id))
		res->stat.field |= IPC_DEV_STAT_VENDOR_ID;

	if (!dev_get_manufacturer(&res->stat.manufacturer))
		res->stat.field |= IPC_DEV_STAT_MANUFACTURER;

	if (!dev_get_product_id(&res->stat.product_id))
		res->stat.field |= IPC_DEV_STAT_PRODUCT_ID;

	if (!dev_get_product(&res->stat.product))
		res->stat.field |= IPC_DEV_STAT_PRODUCT;

	if (!dev_get_device_version(&res->stat.device_version))
		res->stat.field |= IPC_DEV_STAT_DEVICE_VERSION;

	if (!dev_get_serial_number(&res->stat.serial_number))
		res->stat.field |= IPC_DEV_STAT_SERIAL_NUMBER;

	if (!dev_get_usb_version(&res->stat.usb_version))
		res->stat.field |= IPC_DEV_STAT_USB_VERSION;

	if (!dev_get_bus_number(&res->stat.bus_number))
		res->stat.field |= IPC_DEV_STAT_BUS_NUMBER;

	if (!dev_get_device_address(&res->stat.device_address))
		res->stat.field |= IPC_DEV_STAT_DEVICE_ADDRESS;

	if (!dev_get_port(&res->stat.port, &res->stat.port_count))
		res->stat.field |= IPC_DEV_STAT_PORT;

	if (!dev_get_speed(&res->stat.speed))
		res->stat.field |= IPC_DEV_STAT_SPEED;
}

void daemon_exec_req(struct ipc_request *req, struct ipc_response *res)
{
	if (!dev_enabled()) {
		res->type = IPC_RES_ERROR;
		res->error = "device unavailable";
		return;
	}

	switch (req->type) {
	case IPC_REQ_FRAME:
		break;
	case IPC_REQ_CLEAR:
		break;
	case IPC_REQ_STAT:
		stat_device(res);
		break;
	}
}

int cmd_main_start(int argc, const char **argv)
{
	event_init();
	ipc_init_d();
	dev_init();

	dev_setup_pollfd();
	dev_enable_hotplug();

	ipc_listen_d();
	event_start_loop();

	cc_trap();
	return 0;
}
