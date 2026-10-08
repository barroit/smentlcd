// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#include "packet.h"

#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include "atexit.h"
#include "compiler.h"
#include "libdeflate.h"
#include "image.h"
#include "log.h"
#include "playback.h"
#include "rio.h"
#include "xalloc.h"

#define SHM_NAME "/smentlcd_packet"

static void drop_shm(void)
{
	shm_unlink(SHM_NAME);
}

static void packet_open_shm(struct packet *packet, size_t count)
{
	int err;

	packet->nalloc = cc_offsetof(struct playback, frames) +
			 sizeof(struct frame) * count;

	atexit_push(drop_shm);
	packet->fd = shm_open(SHM_NAME, O_CREAT | O_EXCL | O_RDWR,
			      S_IRUSR | S_IWUSR);
	if (packet->fd == -1)
		die_errno("can't create %s", SHM_NAME);

	err = rftruncate(packet->fd, packet->nalloc);
	if (err)
		die_errno("can't set size for %s", SHM_NAME);
}

void packet_alloc_buffer(struct packet *packet, struct image *image)
{
	packet_open_shm(packet, image->count);
	packet->buf = xmmap(NULL, packet->nalloc, PROT_READ | PROT_WRITE,
			    MAP_SHARED, packet->fd, 0);

	packet->playback = packet->buf;
	packet->playback->count = image->count;
	packet->playback->index = 0;
}

static void alloc_compressor(struct packet *packet)
{
	packet->compressor = libdeflate_alloc_compressor(
		CONFIG_IMAGE_COMPRESS_LEVEL);
	if (!packet->compressor)
		die("can't allocate libdeflate compressor");
}

void packet_compress_image(struct packet *packet, struct image *image)
{
	size_t size;
	unsigned int idx;

	alloc_compressor(packet);
	size = image_frame_size(image);
	for (idx = 0; idx < packet->playback->count; idx++) {
		size_t nw;
		struct frame *frame;
		uint8_t *out;

		frame = &packet->playback->frames[idx];
		out = &frame->buf[CONFIG_PACKET_HEADER_SIZE];
		nw = libdeflate_deflate_compress(packet->compressor,
						 &image->buf[size * idx], size,
						 out, CONFIG_PACKET_FRAME_SIZE);
		if (!nw)
			die("frame at %u exceeds %u bytes", idx,
			    CONFIG_PACKET_FRAME_SIZE);

		frame->size = nw + CONFIG_PACKET_HEADER_SIZE;
		frame->delay = image->delays[idx];
	}
}

void packet_populate_header(struct packet *packet)
{
	unsigned int idx;

	for (idx = 0; idx < packet->playback->count; idx++) {
		struct frame *frame;
		uint8_t *header;
		size_t size;

		frame = &packet->playback->frames[idx];
		header = frame->buf;
		size = frame->size - CONFIG_PACKET_HEADER_SIZE;

		/*
		 * Reserved bytes and padding must be zero.
		 */
		memset(header, 0, CONFIG_PACKET_HEADER_SIZE);

		/*
		 * Update mode, full framebuffer replacement.
		 */
		header[0x000] = 0x11;

		/*
		 * Required protocol tag.
		 */
		header[0x001] = 0x0f; 

		/*
		 * Raw DEFLATE payload size, little-endian.
		 */
		header[0x002] = size & 0xff;
		header[0x003] = size >> 8;

		/*
		 * Fixed full-update parameters.
		 */
		header[0x00c] = 0x00;
		header[0x00d] = 0x3f;
		header[0x00e] = 0x01;
		header[0x00f] = 0xef;
	}
}
