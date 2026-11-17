#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-or-later

name=$(printf %s $1 | cut -d/ -f2- | sed -e 's|main/|/|' -e 's|^/||' \
					 -e 's|/|_|g')

cat <<EOF
int cmd_$name(int, const char **);

int main(int argc, const char **argv)
{
	return cmd_$name(argc, argv);
}
EOF
