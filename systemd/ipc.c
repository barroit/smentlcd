// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#include "ipc.h"

#include <assert.h>
#include <poll.h>
#include <systemd/sd-daemon.h>
#include <systemd/sd-event.h>
#include <systemd/sd-json.h>
#include <systemd/sd-varlink.h>

#include "libusb.h"
#include "log.h"
#include "xalloc.h"

#define DEFINE_METHOD_SCHEME static SD_VARLINK_DEFINE_METHOD
#define DEFINE_INPUT_SCHEME SD_VARLINK_DEFINE_INPUT

#define DEFINE_ERROR_SCHEME static SD_VARLINK_DEFINE_ERROR
#define DEFINE_FIELD_SCHEME SD_VARLINK_DEFINE_FIELD

#define DECLARE_METHOD(name)					\
static int method_ ## name(sd_varlink *link,			\
			   sd_json_variant *parameters,		\
			   sd_varlink_method_flags_t flags,	\
			   void *userdata)

DECLARE_METHOD(frame);
DECLARE_METHOD(brightness);
DECLARE_METHOD(clear);
DECLARE_METHOD(power);
DECLARE_METHOD(status);

struct ipc_ctx {
	struct sd_varlink_server *server;
	struct sd_event *event;
	ipc_exec_req_fn exec_req;
};

struct method_map {
	const char *name;
	sd_varlink_method_t method;
};

DEFINE_METHOD_SCHEME(Frame,
		     DEFINE_INPUT_SCHEME(fd, SD_VARLINK_INT, 0));

DEFINE_METHOD_SCHEME(Brightness,
		     DEFINE_INPUT_SCHEME(brightness, SD_VARLINK_INT, 0));

DEFINE_METHOD_SCHEME(Clear);

DEFINE_METHOD_SCHEME(Power,
		     DEFINE_INPUT_SCHEME(on, SD_VARLINK_BOOL, 0));

DEFINE_METHOD_SCHEME(Status);

DEFINE_ERROR_SCHEME(Error,
		    DEFINE_FIELD_SCHEME(error, SD_VARLINK_STRING, 0));

static SD_VARLINK_DEFINE_INTERFACE(scheme, IPC_INTERFACE_NAME,
				   &vl_method_Frame,
				   &vl_method_Brightness,
				   &vl_method_Clear,
				   &vl_method_Power,
				   &vl_method_Status,
				   &vl_error_Error);

static struct method_map methods[] = {
	{ IPC_INTERFACE_NAME ".Frame", method_frame },
	{ IPC_INTERFACE_NAME ".Brightness", method_brightness },
	{ IPC_INTERFACE_NAME ".Clear", method_clear },
	{ IPC_INTERFACE_NAME ".Power", method_power },
	{ IPC_INTERFACE_NAME ".Status", method_status },
	{ NULL, NULL },
};

static int emit_final_reply(sd_varlink *link, struct ipc_response *res)
{
	switch (res->type) {
	case IPC_RES_ERROR:
		return sd_varlink_errorbo(link, IPC_INTERFACE_NAME ".Error",
			SD_JSON_BUILD_PAIR_STRING("error", res->error));
	default:
		return sd_varlink_reply(link, NULL);
	}
}

DECLARE_METHOD(frame)
{
	return 0;
}

DECLARE_METHOD(brightness)
{
	struct ipc_ctx *ctx = userdata;
	struct sd_json_variant *field;
	struct ipc_request req = {
		.type = IPC_REQ_BRIGHTNESS,
	};
	struct ipc_response res = { 0 };

	record("running");

	field = sd_json_variant_by_key(parameters, "brightness");
	req.brightness = sd_json_variant_integer(field);

	ctx->exec_req(&req, &res);
	return emit_final_reply(link, &res);
}

DECLARE_METHOD(clear)
{
	return 0;
}

DECLARE_METHOD(power)
{
	return 0;
}

DECLARE_METHOD(status)
{
	return 0;
}

void ipc_init(struct ipc_ctx **__ctx)
{
	static struct ipc_ctx ctx;
	int err;
	struct method_map *entry = methods;

	assert(!ctx.server);

	err = sd_varlink_server_new(&ctx.server,
				    SD_VARLINK_SERVER_INHERIT_USERDATA |
				    SD_VARLINK_SERVER_ALLOW_FD_PASSING_INPUT);
	if (err)
		die_errno2(-err, "sd_varlink_server_new() failed");

	err = sd_varlink_server_add_interface(ctx.server,
					      &vl_interface_scheme);
	if (err)
		die_errno2(-err, "sd_varlink_server_add_interface() failed");

	sd_varlink_server_set_userdata(ctx.server, &ctx);

	for (; entry->name; entry++) {
		err = sd_varlink_server_bind_method(ctx.server, entry->name,
						    entry->method);
		if (err)
			die_errno2(-err,
				   "sd_varlink_server_bind_method() failed");
	}

	err = sd_event_default(&ctx.event);
	if (err < 0)
		die_errno2(-err, "sd_event_default() failed");

	err = sd_varlink_server_attach_event(ctx.server, ctx.event, 0);
	if (err)
		die_errno2(-err, "sd_varlink_server_attach_event() failed");

	*__ctx = &ctx;
}

void ipc_bind_exec_req(struct ipc_ctx *ctx, ipc_exec_req_fn fn)
{
	ctx->exec_req = fn;
}

void ipc_listen(struct ipc_ctx *ctx)
{
	int ret;

	ret = sd_varlink_server_listen_auto(ctx->server);
	if (ret < 0)
		die_errno2(-ret, "sd_varlink_server_listen_auto() failed");
	else if (ret == 0)
		die("no listening socket acquired");

	ret = sd_event_loop(ctx->event);
	if (ret < 0)
		die_errno2(-ret, "sd_event_loop() failed");
}

static int handle_event_io(sd_event_source *src, int fd, uint32_t revents,
			   void *userdata)
{
	int err;
	struct timeval tv = { 0 };

	err = libusb_handle_events_timeout(NULL, &tv);
	if (err < 0) {
		error_libusb(-err, "libusb cannot handle pending events");
		return -1;
	}

	return 0;
}

void *ipc_watch_pollfd(struct ipc_ctx *ctx, size_t nalloc, int fd, short events)
{
	int err;
	char *buf;
	struct sd_event_source **src;
	uint32_t sd_events = 0;

	if (events & POLLIN)
		sd_events |= EPOLLIN;

	if (events & POLLOUT)
		sd_events |= EPOLLOUT;

	buf = xmalloc(nalloc + sizeof(src));
	src = (typeof(src))&buf[nalloc];

	err = sd_event_add_io(ctx->event, src, fd, sd_events, handle_event_io,
			      ctx);
	if (err) {
		error_errno2(-err,
			     "unable to add libusb pollfd %d as new I/O event source to event loop",
			     fd);
		free(buf);
		return NULL;
	}

	return buf;
}

void ipc_unwatch_pollfd(struct ipc_ctx *ctx, void *src)
{
	sd_event_source_unref((struct sd_event_source *)src);
}
