/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#ifndef STRTOX_H
#define STRTOX_H

#include <stdint.h>

/**
 * strxtoull() - Convert a string to unsigned long long.
 * @str: NUL-terminated string to convert.
 * @base: Numeric base, or zero for automatic detection.
 * @res: Pointer to the converted result.
 *
 * Return: Zero on success, or -1 on failure.
 */
int strxtoull(const char *str, int base, unsigned long long *res);

/**
 * strxtoul() - Convert a string to unsigned long.
 * @str: NUL-terminated string to convert.
 * @base: Numeric base, or zero for automatic detection.
 * @res: Pointer to the converted result.
 *
 * Return: Zero on success, or -1 on failure.
 */
int strxtoul(const char *str, int base, unsigned long *res);

/**
 * strxtou() - Convert a string to unsigned int.
 * @str: NUL-terminated string to convert.
 * @base: Numeric base, or zero for automatic detection.
 * @res: Pointer to the converted result.
 *
 * Return: Zero on success, or -1 on failure.
 */
int strxtou(const char *str, int base, unsigned int *res);

/**
 * strxtoll() - Convert a string to long long.
 * @str: NUL-terminated string to convert.
 * @base: Numeric base, or zero for automatic detection.
 * @res: Pointer to the converted result.
 *
 * Return: Zero on success, or -1 on failure.
 */
int strxtoll(const char *str, int base, long long *res);

/**
 * strxtol() - Convert a string to long.
 * @str: NUL-terminated string to convert.
 * @base: Numeric base, or zero for automatic detection.
 * @res: Pointer to the converted result.
 *
 * Return: Zero on success, or -1 on failure.
 */
int strxtol(const char *str, int base, long *res);

/**
 * strxtos() - Convert a string to int.
 * @str: NUL-terminated string to convert.
 * @base: Numeric base, or zero for automatic detection.
 * @res: Pointer to the converted result.
 *
 * Return: Zero on success, or -1 on failure.
 */
int strxtos(const char *str, int base, int *res);

/**
 * strxtou64() - Convert a string to uint64_t.
 * @str: NUL-terminated string to convert.
 * @base: Numeric base, or zero for automatic detection.
 * @res: Pointer to the converted result.
 *
 * Return: Zero on success, or -1 on failure.
 */
int strxtou64(const char *str, int base, uint64_t *res);

/**
 * strxtou32() - Convert a string to uint32_t.
 * @str: NUL-terminated string to convert.
 * @base: Numeric base, or zero for automatic detection.
 * @res: Pointer to the converted result.
 *
 * Return: Zero on success, or -1 on failure.
 */
int strxtou32(const char *str, int base, uint32_t *res);

/**
 * strxtou16() - Convert a string to uint16_t.
 * @str: NUL-terminated string to convert.
 * @base: Numeric base, or zero for automatic detection.
 * @res: Pointer to the converted result.
 *
 * Return: Zero on success, or -1 on failure.
 */
int strxtou16(const char *str, int base, uint16_t *res);

/**
 * strxtou8() - Convert a string to uint8_t.
 * @str: NUL-terminated string to convert.
 * @base: Numeric base, or zero for automatic detection.
 * @res: Pointer to the converted result.
 *
 * Return: Zero on success, or -1 on failure.
 */
int strxtou8(const char *str, int base, uint8_t *res);

/**
 * strxtos64() - Convert a string to int64_t.
 * @str: NUL-terminated string to convert.
 * @base: Numeric base, or zero for automatic detection.
 * @res: Pointer to the converted result.
 *
 * Return: Zero on success, or -1 on failure.
 */
int strxtos64(const char *str, int base, int64_t *res);

/**
 * strxtos32() - Convert a string to int32_t.
 * @str: NUL-terminated string to convert.
 * @base: Numeric base, or zero for automatic detection.
 * @res: Pointer to the converted result.
 *
 * Return: Zero on success, or -1 on failure.
 */
int strxtos32(const char *str, int base, int32_t *res);

/**
 * strxtos16() - Convert a string to int16_t.
 * @str: NUL-terminated string to convert.
 * @base: Numeric base, or zero for automatic detection.
 * @res: Pointer to the converted result.
 *
 * Return: Zero on success, or -1 on failure.
 */
int strxtos16(const char *str, int base, int16_t *res);

/**
 * strxtos8() - Convert a string to int8_t.
 * @str: NUL-terminated string to convert.
 * @base: Numeric base, or zero for automatic detection.
 * @res: Pointer to the converted result.
 *
 * Return: Zero on success, or -1 on failure.
 */
int strxtos8(const char *str, int base, int8_t *res);

#endif /* STRTOX_H */
