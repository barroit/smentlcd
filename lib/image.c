// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#include "image.h"

#include <ctype.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "log.h"
#include "stb_image.h"
#include "strutil.h"
#include "xalloc.h"

#define stbi_load     stbi_load_from_memory
#define stbi_load_gif stbi_load_gif_from_memory

static enum image_type match_image_type_nocase(const char *suffix)
{
	unsigned int table_idx;
	const char *table[] = {
		"png",
		"jpeg",
		"jpg",
		"gif",
	};
	enum image_type map[] = {
		IMAGE_PNG,
		IMAGE_JPEG,
		IMAGE_JPEG,
		IMAGE_GIF,
	};

	for (table_idx = 0; table_idx < sizeof_array(table); table_idx++) {
		unsigned int idx;
		size_t len;

		len = strlen(table[table_idx]);
		for (idx = 0; idx < len; idx++) {
			if (tolower(suffix[idx]) != table[table_idx][idx])
				goto next;
		}

		if (!suffix[idx])
			return map[table_idx];
next:
	}

	return IMAGE_UNKNOWN;
}

static enum image_type resolve_type_nocase(const char *filename)
{
	const char *suffix;

	suffix = str_seek_suffix(filename, '.');
	if (suffix) {
		enum image_type type;

		type = match_image_type_nocase(suffix);
		if (type != IMAGE_UNKNOWN)
			return type;
	}

	die("unable to probe image type for %s", filename);
}

static void load_single_frame(struct image *image, void *buf, size_t size)
{
	int __channels;
	static unsigned int delays[] = {
		maxof(typeof(*delays)),
	};

	image->buf = stbi_load_from_memory(buf, size, (int *)&image->width,
					   (int *)&image->height, &__channels,
					   STBI_rgb);
	image->count = 1;
	image->delays = delays;
}

static void load_multi_frame(struct image *image, void *buf, size_t size)
{
	int __channels;
	int *__delays = NULL;

	image->buf = stbi_load_gif_from_memory(buf, size, &__delays,
					       (int *)&image->width,
					       (int *)&image->height,
					       (int *)&image->count,
					       &__channels, STBI_rgb);
	image->delays = (unsigned int *)__delays;
}

void image_load(struct image *image, const char *filename)
{
	int err;
	int fd;
	struct stat st;
	void *buf;

	if (image->type == IMAGE_UNKNOWN)
		image->type = resolve_type_nocase(filename);

	fd = open(filename, O_RDONLY);
	if (fd == -1)
		die_errno("can't open image %s", filename);

	err = fstat(fd, &st);
	if (err)
		die_errno("can't stat image %s", filename);

	if (!S_ISREG(st.st_mode))
		die("image %s is not regular file", filename);

	if (st.st_size > maxof(int))
		die("image %s exceeds %u bytes", filename,
		    (unsigned int)maxof(int));

	buf = xmmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	switch (image->type) {
	case IMAGE_PNG:
	case IMAGE_JPEG:
		load_single_frame(image, buf, st.st_size);
		break;
	case IMAGE_GIF:
		load_multi_frame(image, buf, st.st_size);
	}

	if (!image->buf)
		die_stbi("can't process image");

	if (image->width != CONFIG_IMAGE_WIDTH_MAX ||
	    image->height != CONFIG_IMAGE_HEIGHT_MAX)
		die("image pixels must be 640 x 150");

	image->channels = STBI_rgb;

	close(fd);
	munmap(buf, st.st_size);
}

void image_release(struct image *image)
{
	stbi_image_free(image->buf);

	if (image->type == IMAGE_GIF)
		stbi_image_free(image->delays);
}

void image_rgb888_to_bgr565(struct image *image)
{
	size_t idx;
	size_t pixels = image->width * image->height * image->count;
	uint8_t *src = image->buf;
	uint8_t *dst = image->buf;

	for (idx = 0; idx < pixels; idx++) {
		uint8_t r = src[0];
		uint8_t g = src[1];
		uint8_t b = src[2];
		uint16_t pixel;

		pixel = ((uint16_t)(b & 0xf8) << 8) |
			((uint16_t)(g & 0xfc) << 3) |
			((uint16_t)(r) >> 3);

		dst[0] = pixel & 0xff;
		dst[1] = pixel >> 8;

		src += 3;
		dst += 2;
	}

	image->channels = 2;
}
