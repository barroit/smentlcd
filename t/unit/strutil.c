// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#include "strutil.h"
#include "uniassert.h"
#include "unitest.h"

unitest_begin();

unitest_routine(strskip_prefix)
{
	const char s[] = "Hatsune Miku";
	const char *res = str_skip(s, "Hatsune ");

	uniassert_equal(res, s + 8);
	uniassert_strequal(res, "Miku");
	return 0;
}

unitest_routine(strskip_empty)
{
	const char s[] = "miku";
	const char *res = str_skip(s, "");

	uniassert_equal(res, s);
	uniassert_strequal(res, "miku");
	return 0;
}

unitest_routine(strskip_mismatch)
{
	const char s[] = "miku";

	uniassert_not(str_skip(s, "rin"));
	return 0;
}

unitest_routine(strskip_short)
{
	const char s[] = "mi";

	uniassert_not(str_skip(s, "miku"));
	return 0;
}

unitest_routine(strskip2_prefix)
{
	const char s[] = "Hatsune Miku";
	const char *res;
	int err;

	err = str_skip2(s, "Hatsune ", &res);

	uniassert_equal_signed(err, 0);
	uniassert_equal(res, s + 8);
	uniassert_strequal(res, "Miku");
	return 0;
}

unitest_routine(strskip2_mismatch)
{
	const char s[] = "miku";
	const char *res = s;
	int err;

	err = str_skip2(s, "rin", &res);

	uniassert_equal_signed(err, -1);
	uniassert_equal(res, s);
	return 0;
}

unitest_end();
