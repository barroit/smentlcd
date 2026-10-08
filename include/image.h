/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#ifndef IMAGE_H
#define IMAGE_H

#include <stddef.h>
#include <stdint.h>

enum image_type {
	IMAGE_UNKNOWN,
	IMAGE_PNG,
	IMAGE_JPEG,
	IMAGE_GIF,
};

struct image {
	enum image_type type;
	uint8_t *buf;

	unsigned int width;
	unsigned int height;
	unsigned int channels;

	unsigned int count;
	unsigned int *delays;
};

void image_load(struct image *image, const char *filename);

static inline size_t image_frame_size(struct image *image)
{
	return image->width * image->height * image->channels;
}

void image_rgb888_to_bgr565(struct image *image);

#endif /* IMAGE_H */
