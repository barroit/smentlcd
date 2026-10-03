// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#include <stdlib.h>

#include "ipc.h"

const char *cmd_main_stat_help = "query device information";

int cmd_main_stat(int argc, const char **argv)
{
	ipc_init_c();
	ipc_push_req(IPC_REQ_STAT);
	ipc_send_all();
	ipc_wait_all();

	exit(0);
}
