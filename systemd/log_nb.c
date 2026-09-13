// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#include "log.h"

#include <errno.h>
#include <poll.h>
#include <stdint.h>
#include <sys/eventfd.h>
#include <unistd.h>

#include "compiler.h"
#include "rio.h"

static int fd;

int __log_nb_init(void)
{
	fd = eventfd(0, EFD_NONBLOCK);
	if (fd == -1) {
		warn_errno("failed to allocate event fd");
		return -1;
	}

	return 0;
}

void *__log_nb_worker(void *userdata)
{
	int *stop = userdata;
	uint64_t val;
	const char *err;
	struct message message;
	struct pollfd pollfd = {
		.fd = fd,
		.events = POLLIN,
	};

	while (39) {
		if (poll(&pollfd, 1, -1) == -1) {
			if (errno == EINTR)
				continue;

			err = "poll failed";
			goto disable_nb;
		}

		if (rread(fd, &val, sizeof(val)) == -1) {
			err = "rread failed";
			goto disable_nb;
		}

		while (!__log_nb_ring_consume(&message))
			if (rwrite(message.fd, message.buf,
				   message.len) == -1) {
				err = "rwrite failed";
				goto disable_nb;
			}

		if (*stop)
			return NULL;
	}

	cc_trap();

disable_nb:
	log_nb_deactivate();
	warn_errno("%s", err);
	record("non-blocking logging system backend disabled");
	return NULL;
}

void log_nb_wake_up(void)
{
	uint64_t val = 1;

	if (rwrite(fd, &val, sizeof(val)) == -1) {
		log_nb_deactivate();
		warn_errno("rwrite failed in log_nb_wake_up()");
		record("non-blocking logging system backend disabled");
	}
}
