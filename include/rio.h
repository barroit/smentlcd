/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#ifndef RIO_H
#define RIO_H

#include <sys/types.h>

/**
 * rread() - Read up to a specified number of bytes.
 * @fd: File descriptor to read from.
 * @buf: Buffer to receive data.
 * @count: Maximum number of bytes to read.
 *
 * Return: Number of bytes read, or -1 on failure.
 */
ssize_t rread(int fd, void *buf, size_t count);

/**
 * rwrite() - Write up to a specified number of bytes.
 * @fd: File descriptor to write to.
 * @buf: Buffer containing data to write.
 * @count: Maximum number of bytes to write.
 *
 * Return: Number of bytes written, or -1 on failure.
 */
ssize_t rwrite(int fd, const void *buf, size_t count);

/**
 * rftruncate() - Set a file to a specified length.
 * @fd: File descriptor of the file.
 * @length: New file length in bytes.
 *
 * Return: Zero on success, or -1 on failure.
 */
int rftruncate(int fd, off_t length);

#endif /* RIO_H */
