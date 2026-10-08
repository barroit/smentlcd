/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#ifndef PACKET_H
#define PACKET_H

#include <stddef.h>
#include <stdint.h>

struct image;
struct libdeflate_compressor;
struct playback;

struct packet {
	int fd;
	void *buf;
	size_t nalloc;

	struct playback *playback;
	struct libdeflate_compressor *compressor;
};

void packet_alloc_write(struct packet *packet, struct image *image);

void packet_compress_image(struct packet *packet, struct image *image);

void packet_populate_header(struct packet *packet);

#endif /* PACKET_H */
