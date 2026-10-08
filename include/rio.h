/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 *
 * functions with retry on interruption support.
 */

#ifndef RIO_H
#define RIO_H

#include <sys/types.h>

ssize_t rread(int fd, void *buf, size_t count);

ssize_t rwrite(int fd, const void *buf, size_t count);

int rftruncate(int fd, off_t length);

#endif /* RIO_H */
