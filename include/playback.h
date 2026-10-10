/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#ifndef PLAYBACK_H
#define PLAYBACK_H

#include <stddef.h>
#include <stdint.h>

struct frame {
	uint8_t buf[CONFIG_PACKET_HEADER_SIZE + CONFIG_PACKET_FRAME_SIZE];
	size_t size;
	unsigned int delay;
};

struct playback {
	unsigned int count;
	struct frame frames[];
};

void playback_cleanup(void);

int playback_snapshot(int fd);

int playback_sched_loop(void);

#endif /* PLAYBACK_H */
