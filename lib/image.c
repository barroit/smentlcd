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
#include "libdeflate.h"

#define stbi_load     stbi_load_from_memory
#define stbi_load_gif stbi_load_gif_from_memory


struct image {
	enum image_type type;
	void *buf;

	int width;
	int height;
	int channels;

	int *delays;
	int frame_count;
};

struct image_ctx {
	struct image image;
	struct libdeflate_compressor *compressor;
};

static struct image_ctx ctx;

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

void image_init(void)
{
	struct libdeflate_compressor *compressor;
	int level = CONFIG_IMAGE_COMPRESS_LEVEL;

	compressor = libdeflate_alloc_compressor(level);
	if (!compressor)
		die("can't allocate libdeflate compressor");

	ctx.compressor = compressor;
}

void image_load(const char *filename, enum image_type type_hint)
{
	int err;
	int fd;
	struct stat st;
	void *buf;
	int original_channels;

	if (type_hint != IMAGE_UNKNOWN)
		ctx.image.type = type_hint;
	else
		ctx.image.type = resolve_type_nocase(filename);

	fd = open(filename, O_RDONLY);
	if (fd == -1)
		die_errno("can't open image %s", filename);

	err = fstat(fd, &st);
	if (err)
		die_errno("can't stat image %s", filename);

	if (!S_ISREG(st.st_mode))
		die("image %s is not regular file", filename);

	buf = xmmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	switch (ctx.image.type) {
	case IMAGE_PNG:
	case IMAGE_JPEG:
		ctx.image.buf = stbi_load(buf, st.st_size, &ctx.image.width,
					  &ctx.image.height,
					  &original_channels, STBI_rgb);
		ctx.image.frame_count = 1;
		break;
	case IMAGE_GIF:
		ctx.image.buf = stbi_load_gif(buf, st.st_size,
					      &ctx.image.delays,
					      &ctx.image.width,
					      &ctx.image.height,
					      &ctx.image.frame_count,
					      &original_channels, STBI_rgb);
	}

	if (!ctx.image.buf)
		die_stbi("can't process image");

	if (ctx.image.width != CONFIG_IMAGE_WIDTH_MAX ||
	    ctx.image.height != CONFIG_IMAGE_HEIGHT_MAX)
		die("image pixels must be 640 x 150");

	ctx.image.channels = STBI_rgb;

	close(fd);
	munmap(buf, st.st_size);
}

size_t image_frame_size(void)
{
	return ctx.image.width * ctx.image.height * ctx.image.channels;
}

void image_rgb888_to_bgr565(void)
{
	size_t idx;
	size_t pixels = ctx.image.width * ctx.image.height *
			ctx.image.frame_count;
	uint8_t *src = ctx.image.buf;
	uint8_t *dst = ctx.image.buf;

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

	ctx.image.channels = 2;
}
