// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#include <stdlib.h>
#include <string.h>

#include "calc.h"
#include "image.h"
#include "ipc.h"
#include "log.h"
#include "packet.h"
#include "parse_argv.h"

static const char *usage[] = {
	"smentlcdctl play [<options>] <filename>",
	NULL,
};

const char *cmd_main_play_help = "play the image";

int cmd_main_play(int argc, const char **argv)
{
	struct image image = {
		.type = IMAGE_UNKNOWN,
	};
	struct packet packet = { 0 };
	struct pa_opt opts[] = {
		PA_OPT_CMDMODE("png", 0, &image.type, IMAGE_PNG,
			       "treat image as PNG"),
		PA_OPT_CMDMODE("jpeg", 0, &image.type, IMAGE_JPEG,
			       "treat image as JPEG"),
		PA_OPT_CMDMODE("gif", 0, &image.type, IMAGE_GIF,
			       "treat image as GIF"),
		PA_OPT_END(),
	};

	ipc_init_c();

	argc = pa_parse_args(argc, argv, opts, usage, 0);
	if (argc != 1)
		die("single image filename expected");

	image_load(&image, argv[0]);
	image_rgb888_to_bgr565(&image);

	packet_alloc_buffer(&packet, &image);
	packet_compress_image(&packet, &image);
	packet_populate_header(&packet);

	ipc_push_req(IPC_REQ_FRAME, packet.fd);
	ipc_send_all();
	ipc_wait_all();

	exit(0);
}
