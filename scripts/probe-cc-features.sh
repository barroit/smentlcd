#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-or-later

set -e

cc=$1

trap 'rm -f .tmp-$$.*' EXIT

probe_feature()
{
	read name
	read flags

	cat >.tmp-$$.$name

	# Compile and assemble for flag tests only.
	test -s .tmp-$$.$name && mode=-S || mode=-c

	$cc $flags -Werror $mode -x c -o /dev/null .tmp-$$.$name \
	    2>/dev/null && printf '%s=y\n' $name || true
}

probe_feature <<'EOF' &
CC_HAS_TOPLEVEL_REORDER
-ftoplevel-reorder
EOF

probe_feature <<'EOF' &
CC_HAS_GZ_ZLIB
-g -gz=zlib
EOF

probe_feature <<'EOF' &
CC_HAS_GZ_ZSTD
-g -gz=zstd
EOF

probe_feature <<'EOF' &
CC_HAS_BUILTIN_ALIGN_UP

int x = __builtin_align_up(8, 4);
EOF

probe_feature <<'EOF' &
CC_HAS_BUILTIN_ALIGN_DOWN

int x = __builtin_align_down(8, 4);
EOF

probe_feature <<'EOF' &
CC_HAS_REALLOCARRAY
-D_GNU_SOURCE

#include <stdlib.h>

static void a(void)
{
	char *p = reallocarray(NULL, sizeof(*p), 5);

	if (p);
}
EOF

probe_feature <<'EOF' &
CC_HAS_STRCHRNUL
-D_GNU_SOURCE

#include <string.h>

static void a(void)
{
	char *p = strchrnul("miku", 'x');

	if (p);
}
EOF

probe_feature <<'EOF' &
CC_HAS_SSIZE_T
-D_GNU_SOURCE

#include <sys/types.h>

ssize_t a = 0;
EOF

probe_feature <<'EOF' &
CC_HAS_VARIABLE_NO_SANITIZE
-fsanitize=address,undefined

extern int *var;

__attribute__((no_sanitize("address"))) int *var = (typeof(var))39;
EOF

wait
