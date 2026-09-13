// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#include "log.h"

#include <pthread.h>
#include <string.h>
#include <sys/mman.h>

#include "barrier.h"
#include "compiler.h"
#include "rio.h"

struct ring {
	struct message messages[CONFIG_LOG_MESSAGE_RING_SIZE];

	size_t head;
	size_t tail;
};

static struct ring *ring;
static pthread_t worker;

static int auto_commit = 1;
static int stop = 0;

int __log_nb_ring_produce(int fd, const char *prefix, const char *hint,
			  const char *fmt, va_list ap)
{
	size_t head;
	size_t next;
	struct message *message;

	head = READ_ONCE(ring->head);
	next = head + 1;

	if (next == smp_load_acquire(&ring->tail))
		return -1;

	message = &ring->messages[head % CONFIG_LOG_MESSAGE_RING_SIZE];
	message->fd = fd;
	message->len = __log_format_line(message->buf, sizeof(message->buf),
					 prefix, hint, fmt, ap);

	smp_store_release(&ring->head, next);
	return 0;
}

int __log_nb_ring_consume(struct message *message)
{
	size_t tail;
	size_t next;

	tail = READ_ONCE(ring->tail);
	next = tail + 1;

	if (tail == smp_load_acquire(&ring->head))
		return -1;

	message->fd = ring->messages[tail].fd;
	message->len = ring->messages[tail].len;
	memcpy(message->buf, ring->messages[tail].buf, message->len);

	smp_store_release(&ring->tail, next);
	return 0;
}

int log_nb_init(void)
{
	int err;

	err = pthread_create(&worker, NULL, __log_nb_worker, &stop);
	if (err) {
		warn_errno("can't create worker thread for non-blocking logging system backend");
		return -1;
	}

	ring = mmap(NULL, sizeof(*ring), PROT_READ | PROT_WRITE,
		    MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (ring == MAP_FAILED) {
		warn_errno("failed to allocate ring buffer for non-blocking logging system backend");
		return -1;
	}

	ring->head = 0;
	ring->tail = 0;

	return __log_nb_init();
}

void log_nb_activate(void)
{
	__log_vwritef = log_nb_vwritef;
	__log_nb_stop = log_nb_stop;
}

void log_nb_deactivate(void)
{
	__log_vwritef = log_vwritef;
	__log_nb_stop = NULL;
}

void log_nb_auto_commit(int enabled)
{
	auto_commit = enabled;
}

void log_nb_stop(void)
{
	stop = 1;
	log_nb_wake_up();
	pthread_join(worker, NULL);
}

void log_nb_vwritef(int fd, const char *prefix, const char *hint,
		    const char *fmt, va_list ap)
{
	int full;

	full = __log_nb_ring_produce(fd, prefix, hint, fmt, ap);
	if (unlikely(full)) {
		log_vwritef(fd, prefix, hint, fmt, ap);
		goto wake_up;
	}

	if (auto_commit)
wake_up:
		log_nb_wake_up();
}
