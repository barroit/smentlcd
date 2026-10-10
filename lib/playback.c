// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#include "playback.h"

#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "device.h"
#include "event.h"
#include "log.h"

struct playback_ctx {
	struct playback *playback;
	size_t nalloc;

	unsigned int index;
	void *timer;
};

static struct playback_ctx ctx;

void playback_cleanup(void)
{
	if (ctx.playback)
		munmap(ctx.playback, ctx.nalloc);

	if (ctx.timer)
		event_destroy_timeout(ctx.timer);

	ctx.playback = NULL;
	ctx.timer = NULL;
}

int playback_snapshot(int fd)
{
	int err;
	struct stat st;
	void *ptr;
	void *snapshot;

	err = fstat(fd, &st);
	if (err) {
		error_errno("can't read shared memory object");
		return -1;
	}

	ptr = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (ptr == MAP_FAILED) {
		error_errno("can't map shared memory object");
		return -1;
	}

	snapshot = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE,
			MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (snapshot == MAP_FAILED) {
		error_errno("can't allocate playback snapshot");
		munmap(ptr, st.st_size);
		return -1;
	}

	memcpy(snapshot, ptr, st.st_size);
	munmap(ptr, st.st_size);

	ctx.playback = snapshot;
	ctx.nalloc = st.st_size;
	return 0;
}

static int loop_frame(void)
{
	int err;
	struct frame *frame;

	if (!dev_enabled())
		return -1;

	frame = &ctx.playback->frames[ctx.index];
	err = dev_submit_frame(frame->buf, frame->size, ctx.index);
	if (err)
		return -1;

	ctx.index = (ctx.index + 1) % ctx.playback->count;
	return event_resched_timeout(ctx.timer, frame->delay);
}

int playback_sched_loop(void)
{
	ctx.index = 0;
	return event_sched_timeout(&ctx.timer, 0, loop_frame);
}
