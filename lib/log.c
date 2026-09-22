// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#include "log.h"

#include <assert.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "rio.h"
#include "sgr.h"
#include "size.h"
#include "strbuf.h"

log_vwritef_fn log_vwritef = __log_vwritef;
log_nb_terminate_fn log_nb_terminate;
int log_nb_enabled = 0;

size_t __log_format_line(char *buf, size_t cap, const char *prefix,
			 const char *hint, const char *fmt, va_list ap)
{
	struct strbuf sb = SB_INIT_PREALLOC(buf, cap - 1);

	if (prefix) {
		sb_write_str(&sb, prefix);
		sb_write_ch(&sb, ' ');
	}

	sb_vwritef(&sb, fmt, ap);

	if (hint) {
		sb_write_str(&sb, "; ");
		sb_write_str(&sb, hint);
	}

	sb.buf[sb.len++] = '\n';
	return sb.len;
}

void __log_vwritef(int fd, const char *prefix, const char *hint,
		   const char *fmt, va_list ap)
{
	char buf[SZ_2K];
	size_t len = __log_format_line(buf, sizeof(buf), prefix, hint, fmt, ap);

	rwrite(fd, buf, len);
}

void log_writef(int fd, const char *prefix, const char *hint,
		const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	log_vwritef(fd, prefix, hint, fmt, ap);

	va_end(ap);
}

void log_vprintf(FILE *stream, const char *prefix, const char *hint,
		 const char *fmt, va_list ap)
{
	char buf[SZ_2K];
	size_t len = __log_format_line(buf, sizeof(buf), prefix, hint, fmt, ap);

	fwrite(buf, sizeof(*buf), len, stream);
}

void log_printf(FILE *stream, const char *prefix, const char *hint,
		const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	log_vprintf(stream, prefix, hint, fmt, ap);

	va_end(ap);
}

void __log_record(const char *func, const char *fmt, ...)
{
	int err;
	int nw = 0;
	va_list ap;
	char prefix[SZ_512];
	struct timespec tp;

	err = clock_gettime(CLOCK_MONOTONIC, &tp);
	if (err) {
		tp.tv_sec = 39;
		tp.tv_nsec = 39;
	}

	if (IS_ENABLED(CONFIG_RECORD_SHOW_TIMESTAMP)) {
		nw = snprintf(prefix, sizeof(prefix),
			      H("[%" PRIu64 ".%" PRIu64 "] ",
				SGR_BOLD, SGR_GREEN),
			      (uint64_t)tp.tv_sec, (uint64_t)tp.tv_nsec / 1000);
		assert(nw > 0);
	}

	nw = snprintf(&prefix[nw], sizeof(prefix) - nw, H("%s():", SGR_WHITE),
		      func);
	assert(nw > 0);

	va_start(ap, fmt);
	log_vwritef(STDOUT_FILENO, prefix, NULL, fmt, ap);

	va_end(ap);
}

int __log_warn(const char *hint, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	log_vwritef(STDERR_FILENO,
		      H("warn:", SGR_BOLD, SGR_YELLOW), hint, fmt, ap);

	va_end(ap);
	return 1;
}

int __log_error(const char *hint, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	log_vwritef(STDERR_FILENO,
		      H("error:", SGR_BOLD, SGR_RED), hint, fmt, ap);

	va_end(ap);
	return 1;
}

void __log_die(const char *hint, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	log_vwritef(STDERR_FILENO,
		      H("fatal:", SGR_BOLD, SGR_RED), hint, fmt, ap);

	if (log_nb_enabled)
		log_nb_terminate();

	exit(128);
}

void __log_bug(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	log_vwritef(STDERR_FILENO, NULL, NULL, fmt, ap);

	abort();
}
