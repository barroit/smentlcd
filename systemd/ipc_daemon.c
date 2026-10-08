// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#include "ipc.h"

#include <assert.h>
#include <systemd/sd-daemon.h>
#include <systemd/sd-json.h>
#include <systemd/sd-varlink.h>
#include <unistd.h>

#include "event.h"
#include "log.h"

void daemon_exec_req(struct ipc_request *req, struct ipc_response *res);

#define DEFINE_METHOD_SCHEME static SD_VARLINK_DEFINE_METHOD
#define DEFINE_OUTPUT_SCHEME SD_VARLINK_DEFINE_OUTPUT

#define DEFINE_ERROR_SCHEME static SD_VARLINK_DEFINE_ERROR
#define DEFINE_FIELD_SCHEME SD_VARLINK_DEFINE_FIELD

#define BUILD_PAIR_UNSIGNED   SD_JSON_BUILD_PAIR_UNSIGNED
#define BUILD_PAIR_STRING     SD_JSON_BUILD_PAIR_STRING
#define BUILD_PAIR_BYTE_ARRAY SD_JSON_BUILD_PAIR_BYTE_ARRAY

#define DECLARE_METHOD(name)					\
static int method_ ## name(sd_varlink *link,			\
			   sd_json_variant *parameters,		\
			   sd_varlink_method_flags_t flags,	\
			   void *userdata)

DECLARE_METHOD(frame);
DECLARE_METHOD(clear);
DECLARE_METHOD(stat);

struct ipc_ctx {
	struct sd_varlink_server *server;
};

static struct ipc_ctx ctx;

struct method_map {
	const char *name;
	sd_varlink_method_t method;
};

DEFINE_METHOD_SCHEME(Frame);

DEFINE_METHOD_SCHEME(Clear);

DEFINE_METHOD_SCHEME(Stat,
		     DEFINE_OUTPUT_SCHEME(field, SD_VARLINK_INT, 0),
		     DEFINE_OUTPUT_SCHEME(vendor_id, SD_VARLINK_STRING, 0),
		     DEFINE_OUTPUT_SCHEME(manufacturer, SD_VARLINK_STRING, 0),

		     DEFINE_OUTPUT_SCHEME(product_id, SD_VARLINK_STRING, 0),
		     DEFINE_OUTPUT_SCHEME(product, SD_VARLINK_STRING, 0),

		     DEFINE_OUTPUT_SCHEME(device_version, SD_VARLINK_STRING, 0),
		     DEFINE_OUTPUT_SCHEME(serial_number, SD_VARLINK_STRING, 0),
		     DEFINE_OUTPUT_SCHEME(usb_version, SD_VARLINK_STRING, 0),

		     DEFINE_OUTPUT_SCHEME(bus_number, SD_VARLINK_INT, 0),
		     DEFINE_OUTPUT_SCHEME(device_address, SD_VARLINK_INT, 0),
		     DEFINE_OUTPUT_SCHEME(port, SD_VARLINK_INT,
					  SD_VARLINK_ARRAY),
		     DEFINE_OUTPUT_SCHEME(speed, SD_VARLINK_STRING, 0)
);

DEFINE_ERROR_SCHEME(Error,
		    DEFINE_FIELD_SCHEME(error, SD_VARLINK_STRING, 0));

static SD_VARLINK_DEFINE_INTERFACE(scheme, IPC_INTERFACE_NAME,
				   &vl_method_Frame,
				   &vl_method_Clear,
				   &vl_method_Stat,
				   &vl_error_Error);

static struct method_map methods[] = {
	{ IPC_INTERFACE_NAME ".Frame", method_frame },
	{ IPC_INTERFACE_NAME ".Clear", method_clear },
	{ IPC_INTERFACE_NAME ".Stat",  method_stat  },
	{ NULL, NULL },
};

static int emit_stat_reply(sd_varlink *link, struct ipc_dev_stat *stat)
{
	return sd_varlink_replybo(link,
	BUILD_PAIR_UNSIGNED("field", stat->field),

	BUILD_PAIR_STRING("vendor_id", stat->vendor_id),
	BUILD_PAIR_STRING("manufacturer", stat->manufacturer),

	BUILD_PAIR_STRING("product_id", stat->product_id),
	BUILD_PAIR_STRING("product", stat->product),

	BUILD_PAIR_STRING("device_version", stat->device_version),
	BUILD_PAIR_STRING("serial_number", stat->serial_number),
	BUILD_PAIR_STRING("usb_version", stat->usb_version),

	BUILD_PAIR_UNSIGNED("bus_number", stat->bus_number),
	BUILD_PAIR_UNSIGNED("device_address", stat->device_address),
	BUILD_PAIR_BYTE_ARRAY("port", stat->port, stat->port_count),

	BUILD_PAIR_STRING("speed", stat->speed));
}

static int emit_error_reply(sd_varlink *link, const char *error)
{
	return sd_varlink_errorbo(link, IPC_INTERFACE_NAME ".Error",
				  SD_JSON_BUILD_PAIR_STRING("error", error));
}

static int emit_reply(sd_varlink *link, struct ipc_response *res)
{
	switch (res->type) {
	case IPC_RES_STAT:
		return emit_stat_reply(link, &res->stat);
	case IPC_RES_ERROR:
		return emit_error_reply(link, res->error);
	default:
		return sd_varlink_reply(link, NULL);
	}
}

DECLARE_METHOD(frame)
{
	struct ipc_request req = {
		.type = IPC_REQ_FRAME,
	};
	struct ipc_response res = { 0 };

	req.fd = sd_varlink_take_fd(link, 0);
	if (req.fd < 0)
		return emit_error_reply(link,
					"can't receive frame file descriptor");

	record("running");
	daemon_exec_req(&req, &res);

	close(req.fd);
	return emit_reply(link, &res);
}

DECLARE_METHOD(clear)
{
	return sd_varlink_reply(link, NULL);
}

DECLARE_METHOD(stat)
{
	struct ipc_request req = {
		.type = IPC_REQ_STAT,
	};
	struct ipc_response res = { 0 };

	record("running");
	daemon_exec_req(&req, &res);

	return emit_reply(link, &res);
}

void ipc_init_d(void)
{
	int err;
	struct method_map *entry;

	assert(!ctx.server);

	err = sd_varlink_server_new(&ctx.server,
				    SD_VARLINK_SERVER_ALLOW_FD_PASSING_INPUT);
	if (err)
		die_errno2(-err, "sd_varlink_server_new() failed");

	err = sd_varlink_server_add_interface(ctx.server,
					      &vl_interface_scheme);
	if (err)
		die_errno2(-err, "sd_varlink_server_add_interface() failed");

	for (entry = methods; entry->name; entry++) {
		err = sd_varlink_server_bind_method(ctx.server, entry->name,
						    entry->method);
		if (err)
			die_errno2(-err,
				   "sd_varlink_server_bind_method() failed");
	}

	err = sd_varlink_server_attach_event(ctx.server, ev_current, 0);
	if (err)
		die_errno2(-err, "sd_varlink_server_attach_event() failed");
}

void ipc_listen_d(void)
{
	int ret;

	ret = sd_varlink_server_listen_auto(ctx.server);
	if (ret < 0)
		die_errno2(-ret, "sd_varlink_server_listen_auto() failed");
	else if (ret == 0)
		die("no listening socket acquired");
}
