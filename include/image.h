/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#ifndef IMAGE_H
#define IMAGE_H

#include <stddef.h>

enum image_type {
	IMAGE_UNKNOWN,
	IMAGE_PNG,
	IMAGE_JPEG,
	IMAGE_GIF,
};

void image_init(void);

void image_load(const char *filename, enum image_type type_hint);

size_t image_frame_size(void);

void image_rgb888_to_bgr565(void);

#endif /* IMAGE_H */
