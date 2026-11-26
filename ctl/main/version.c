// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#include <stdio.h>
#include <stdlib.h>

const char *help_version = "show version information";

int cmd_version(int argc, const char **argv)
{
	puts(BUILD_REPO_NAME " " BUILD_REPO_VERSION);

	printf("worktree %.12s", WORKTREE_HEAD);
	if (*WORKTREE_DIRTY)
		printf(" (%.12s)\n", WORKTREE_DIRTY);
	else
		puts(" (clean)");

	exit(0);
}
