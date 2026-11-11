// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "atexit.h"
#include "size.h"
#include "unitest.h"

static int require_deps(const char *program, const char *dump)
{
	char buf[SZ_4K];
	char cp[SZ_4K];
	size_t cap = sizeof(buf);
	FILE *stream = fopen(dump, "r");
	unsigned int missing = 0;
	char *test;

	while (test = fgets(buf, cap, stream)) {
		char *lf = strchr(buf, '\n');
		char *sep = strrchr(buf, '/');
		struct stat st;

		assert(lf);
		*lf = 0;

		if (!*buf || !stat(buf, &st))
			continue;

		switch (missing) {
		case 0:
			printf("0 # skip %s as", program);
			break;
		case 1:
			printf(" %s", cp);
			break;
		default:
			printf(", %s", cp);
		}

		strcpy(cp, &sep[1]);
		missing++;
	}

	switch (missing) {
	case 0:
		return 0;
	case 1:
		break;
	case 2:
		fputs(" and", stdout);
		break;
	default:
		fputs(", and", stdout);
	}

	printf(" %s", cp);
	printf(" %s missing\n", missing == 1 ? "is" : "are");
	return 1;
}

int main(int argc, const char **argv)
{
	unsigned int idx;
	unsigned int stop = unitest_end - unitest_begin - 1;
	unitest_routine_t *tests = (typeof(tests))unitest_begin + 1;

	atexit_setup();

	setvbuf(stdout, NULL, _IOLBF, 0);
	setvbuf(stderr, NULL, _IOLBF, 0);

	puts("TAP version 14");
	fputs("1..", stdout);

	if (argc == 2 && require_deps(argv[0], argv[1]))
		exit(1);

	if (!stop) {
		printf("%u # skip %s as no test found\n", stop, argv[0]);
		exit(0);
	}

	printf("%u # %s\n", stop, argv[0]);

	if (unitest_teardown)
		atexit_push(unitest_teardown);

	if (unitest_setup)
		unitest_setup();

	/*
	 * FIXME: support MT
	 */
	for (idx = 0; idx < stop; idx++) {
		struct unitest unitest = { 0 };
		int err = tests[idx](&unitest);
		const char *not = err ? "not " : "";

		printf("%sok %u", not, idx + 1);

		if (unitest.detail)
			printf(" - %s", unitest.detail);

		if (unitest.direct) {
			fputs(" # %s", stdout);

			switch (unitest.direct) {
			case UNITEST_TODO:
				fputs("TODO", stdout);
			case UNITEST_SKIP:
				fputs("SKIP", stdout);
			}

			if (unitest.direct_reason)
				printf(" %s", unitest.direct_reason);
		}

		putchar('\n');
	}

	exit(0);
}
