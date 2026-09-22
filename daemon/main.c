// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#include "compiler.h"
#include "log.h"
#include "log_nb.h"
#include "parse_argv.h"

#include "stdlib.h"

static const char *usage[] = {
	"smentlcd <command> [<args>]",
	NULL,
};

int cmd_main(int argc, const char **argv)
{
	int err;
	pa_command_fn cmd;
	struct pa_opt opts[] = {
		CMD_MAIN_CMDS(&cmd),
		PA_OPT_END(),
	};

	err = log_nb_catch_signal();
	if (!err) {
		err = log_nb_init();
		if (err)
			log_nb_restore_signal();
	}

	if (err) {
		record("not using non-blocking logging system backend");
	} else {
		log_nb_activate();
		record("non-blocking logging system backend enabled");
	}

	argc = pa_parse_args(argc, argv, opts, usage, 0);
	cmd(argc, argv);

	cc_trap();
}
