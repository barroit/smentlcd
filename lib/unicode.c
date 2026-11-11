// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#include <stdlib.h>
#include <locale.h>

#include "calc.h"
#include "log.h"
#include "unicode.h"

unsigned int _uc_mbctype[] = {
	[0x00 ... 0x7f] = _UC_9A,
	[0x80 ... 0xbf] = _UC_9CB,
	[0xc0 ... 0xdf] = _UC_9B,
	[0xe0 ... 0xef] = _UC_9C,
	[0xf0 ... 0xf7] = _UC_9D,
	[0xf8 ... 0xff] = -1,
};

static const wchar_t punct[] = {
	0x0021, /* EXCLAMATION MARK */
	0x002c, /* COMMA */
	0x002e, /* FULL STOP */
	0x003f, /* QUESTION MARK */
	0x2025, /* TWO DOT LEADER */
	0x2026, /* HORIZONTAL ELLIPSIS */
	0x3001, /* IDEOGRAPHIC COMMA */
	0x3002, /* IDEOGRAPHIC FULL STOP */
	0xff01, /* FULLWIDTH EXCLAMATION MARK */
	0xff0c, /* FULLWIDTH COMMA */
	0xff0e, /* FULLWIDTH FULL STOP */
	0xff1f, /* FULLWIDTH QUESTION MARK */
};

static int try_c_locales(void)
{
	unsigned int idx;
	const char *cands[] = {
		"C.UTF-8",
		"C.UTF8",
		"C.utf-8",
		"C.utf8",
	};

	for (idx = 0; idx < sizeof_array(cands); idx++) {
		if (setlocale(LC_CTYPE, cands[idx]))
			return 0;
	}

	return 1;
}

void uc_force_utf8_ctype(void)
{
	const char *ctype = getenv("LC_CTYPE");

	if (ctype) {
		if (!setlocale(LC_CTYPE, ctype))
			warn("invalid 'LC_CTYPE' found in env");
		else
			return;
	}

	if (setlocale(LC_CTYPE, CONFIG_UTF8_LOCALE))
		return;

	if (!try_c_locales())
		return;

	__log_die("either install one, or specify a valid one via LC_CTYPE",
		  "can't set locale to UTF-8");
}

size_t uc_mbslen(const char *s)
{
	size_t len = 0;

	while (*s) {
		s += uc_mbctype(*s);
		len++;
	}

	return len;
}

wchar_t uc_mbtowc(const char *seq)
{
	unsigned int shift = 6;
	unsigned int mask = 0x1f;
	wchar_t res = 0;
	unsigned int len = uc_mbctype(*seq);

	switch (len) {
	case _UC_9D:
		res |= seq[3] & 0x3f;
		shift += 6;
		mask >>= 1;
	case _UC_9C:
		res |= (seq[2] & 0x3f) << (shift - 6);
		shift += 6;
		mask >>= 1;
	case _UC_9B:
		res |= ((seq[0] & mask) << shift) |
		       ((seq[1] & 0x3f) << (shift - 6));
		break;
	default:
		res = seq[0];
	}

	return res;
}

int uc_iseoc(wchar_t c)
{
	size_t l = 0;
	size_t r = sizeof_array(punct);

	while (l < r) {
		size_t m = (l + r) >> 1;

		if (c < punct[m])
			r = m;
		else if (c > punct[m])
			l = m + 1;
		else
			return 1;
	}

	return 0;
}
