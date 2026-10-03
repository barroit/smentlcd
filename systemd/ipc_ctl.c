// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#include "ipc.h"

#include <stdlib.h>
#include <systemd/sd-path.h>
#include <systemd/sd-varlink.h>
#include <time.h>

#include "log.h"
#include "list.h"
#include "size.h"
#include "xalloc.h"

#define __stat_field(name, type, func) {	\
	#name,					\
	SD_JSON_VARIANT_##type,			\
	sd_json_dispatch_##func,		\
	offsetof(struct ipc_dev_stat, name),	\
	SD_JSON_MANDATORY,			\
}

struct ipc_task {
	struct ipc_request req;
	struct list_head list;

	unsigned int idx;
	struct timespec sent;
};

struct ipc_ctx {
	char *socket;
	sd_varlink *link;

	struct list_head task_queue;
};

static int sd_json_dispatch_port(const char *name, sd_json_variant *variant,
				 sd_json_dispatch_flags_t flags,
				 void *userdata);

static struct ipc_ctx ctx;

static const char *method_table[__IPC_REQ_MAX] = {
	[IPC_REQ_FRAME] = IPC_INTERFACE_NAME ".Frame",
	[IPC_REQ_CLEAR] = IPC_INTERFACE_NAME ".Clear",
	[IPC_REQ_STAT]  = IPC_INTERFACE_NAME ".Stat",
};

static const sd_json_dispatch_field stat_field_table[] = {
	__stat_field(field, UNSIGNED, uint32),

	__stat_field(vendor_id, STRING, const_string),
	__stat_field(manufacturer, STRING, const_string),

	__stat_field(product_id, STRING, const_string),
	__stat_field(product, STRING, const_string),

	__stat_field(device_version, STRING, const_string),
	__stat_field(serial_number, STRING, const_string),
	__stat_field(usb_version, STRING, const_string),

	__stat_field(bus_number, UNSIGNED, uint8),
	__stat_field(device_address, UNSIGNED, uint8),
	__stat_field(port, ARRAY, port),

	__stat_field(speed, STRING, const_string),

	{},
};

static int sd_json_dispatch_port(const char *name, sd_json_variant *variant,
				 sd_json_dispatch_flags_t flags, void *userdata)
{
	uint8_t **port = userdata;
	struct ipc_dev_stat *stat;
	size_t count;
	size_t idx;
	int ret;

	if (!sd_json_variant_is_array(variant))
		return -EINVAL;

	count = sd_json_variant_elements(variant);
	if (count > SZ_16)
		return -EINVAL;

	for (idx = 0; idx < count; idx++) {
		sd_json_variant *obj;

		obj = sd_json_variant_by_index(variant, idx);
		ret = sd_json_dispatch_uint8(name, obj, flags, &(*port)[idx]);
		if (ret < 0)
			return ret;
	}

	stat = container_of(port, struct ipc_dev_stat, port);
	stat->port_count = count;
	return 0;
}

static void recv_stat_reply(struct ipc_task *task, sd_json_variant *reply)
{
	int err;
	uint8_t port_buf[SZ_16] = { 0 };
	struct ipc_dev_stat stat = {
		.port = port_buf,
	};
	size_t idx;

	err = sd_json_dispatch(reply, stat_field_table, 0, &stat);
	if (err < 0)
		die_errno2(-err, "can't parse %s reply",
			   method_table[task->req.type]);

	if (stat.field & IPC_DEV_STAT_VENDOR_ID)
		printf("Vendor ID: %s\n", stat.vendor_id);
	if (stat.field & IPC_DEV_STAT_MANUFACTURER)
		printf("Manufacturer: %s\n", stat.manufacturer);

	if (stat.field & IPC_DEV_STAT_PRODUCT_ID)
		printf("Product ID: %s\n", stat.product_id);
	if (stat.field & IPC_DEV_STAT_PRODUCT)
		printf("Product: %s\n", stat.product);

	if (stat.field & IPC_DEV_STAT_DEVICE_VERSION)
		printf("Device Version: %s\n", stat.device_version);
	if (stat.field & IPC_DEV_STAT_SERIAL_NUMBER)
		printf("Serial Number: %s\n", stat.serial_number);
	if (stat.field & IPC_DEV_STAT_USB_VERSION)
		printf("USB Version: %s\n", stat.usb_version);

	if (stat.field & IPC_DEV_STAT_BUS_NUMBER)
		printf("Bus Number: %" PRIu8 "\n", stat.bus_number);
	if (stat.field & IPC_DEV_STAT_DEVICE_ADDRESS)
		printf("Device Address: %" PRIu8 "\n", stat.device_address);
	if (stat.field & IPC_DEV_STAT_PORT) {
		printf("Port: ");
		for (idx = 0; idx < stat.port_count; idx++)
			printf("%s%" PRIu8, !idx ? "" : " . ", stat.port[idx]);
		putchar('\n');
	}

	if (stat.field & IPC_DEV_STAT_SPEED)
		printf("Speed: %s\n", stat.speed);
}

static int recv_reply(sd_varlink *link, sd_json_variant *reply,
		      const char *failed, sd_varlink_reply_flags_t flags,
		      void *userdata)
{
	struct ipc_task *task;

	task = list_first_entry(&ctx.task_queue, struct ipc_task, list);

	if (failed) {
		sd_json_variant *obj = sd_json_variant_by_key(reply, "error");
		const char *mesg = sd_json_variant_string(obj);

		__log_die(mesg,
			  "%s failed at runtime [%u:%" PRIu64 ".%" PRIu64 "]",
			  method_table[task->req.type], task->idx,
			  (uint64_t)task->sent.tv_sec,
			  (uint64_t)task->sent.tv_nsec / 1000);
	}

	switch (task->req.type) {
	case IPC_REQ_STAT:
		recv_stat_reply(task, reply);
		break;
	}

	list_del(&task->list);
	free(task);

	return 0;
}

void ipc_init_c(void)
{
	int err;

	err = sd_path_lookup(SD_PATH_USER_RUNTIME, CONFIG_IPC_SOCKET_NAME,
			     &ctx.socket);
	if (err < 0)
		die_errno2(-err, "failed to resolve IPC socket path");

	err = sd_varlink_connect_address(&ctx.link, ctx.socket);
	if (err < 0)
		die_errno2(-err, "can't connect to daemon via socket %s",
			   ctx.socket);

	err = sd_varlink_bind_reply(ctx.link, recv_reply);
	if (err < 0)
		die_errno2(-err, "can't bind varlink reply callback");

	list_head_init(&ctx.task_queue);
}

void ipc_push_req(enum ipc_request_type type, ...)
{
	struct ipc_task *task;
	static size_t cnt;

	task = xmalloc(sizeof(*task));
	task->req.type = type;
	task->idx = cnt;
	task->sent.tv_sec = 39;
	task->sent.tv_nsec = 39;

	switch (type) {
	case IPC_REQ_STAT:
		;
	}

	list_add_tail(&task->list, &ctx.task_queue);
	cnt++;
}

void ipc_send_all(void)
{
	struct ipc_task *task;

	list_foreach_entry(task, &ctx.task_queue, list) {
		int err;
		const char *method = method_table[task->req.type];

		err = sd_varlink_invoke(ctx.link, method, NULL);
		if (err < 0)
			die_errno2(-err, "can't call method %s", method);

		err = clock_gettime(CLOCK_REALTIME, &task->sent);
		if (err)
			warn("failed to record timestamp for task");
	}
}

void ipc_wait_all(void)
{
	while (!list_is_empty(&ctx.task_queue)) {
		int ret;

		ret = sd_varlink_process(ctx.link);
		if (ret < 0)
			die_errno2(-ret, "can't process varlink connection");

		if (ret > 0)
			continue;

		ret = sd_varlink_wait(ctx.link, CONFIG_IPC_REQ_TIMEOUT);
		if (ret < 0) {
			die_errno2(-ret, "can't wait for varlink connection");
		} else if (!ret) {
			struct ipc_task *task;

			task = list_first_entry(&ctx.task_queue,
						struct ipc_task, list);
			die("varlink request for %s timed out [%u:%" PRIu64 ".%" PRIu64 "]",
			    method_table[task->req.type], task->idx,
			    (uint64_t)task->sent.tv_sec,
			    (uint64_t)task->sent.tv_nsec / 1000);
		}
	}
}
