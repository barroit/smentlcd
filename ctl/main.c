// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#include "atexit.h"
#include "compiler.h"
#include "parse_argv.h"
#include "unicode.h"

static const char *usage[] = {
	"smentlcdctl <command> [<args>]",
	NULL,
};

int cmd_main(int argc, const char **argv)
{
	pa_command_fn cmd;
	struct pa_opt opts[] = {
		CMD_MAIN_CMDS(&cmd),
		PA_OPT_END(),
	};

	atexit_setup();
	uc_force_utf8_ctype();

	argc = pa_parse_args(argc, argv, opts, usage, 0);
	cmd(argc, argv);

	cc_trap();
}
