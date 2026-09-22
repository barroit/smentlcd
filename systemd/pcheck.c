// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#include "compiler.h"
#include "log.h"

#include <systemd/sd-id128.h>

static void attr_constructor require_systemd(void)
{
	int err;
	sd_id128_t id;
	char buf[SD_ID128_UUID_STRING_MAX];

	err = sd_id128_get_invocation(&id);
	if (err == -ENXIO)
		die("start this daemon via systemd(1)");
	else if (err < 0)
		die_errno2(-err, "failed to get invocation id");

	sd_id128_to_uuid_string(id, buf);
	record("systemd invocation ID is %s", buf);
}
