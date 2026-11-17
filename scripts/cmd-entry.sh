#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-or-later

cat <<EOF
int cmd_$1(int, const char **);

int main(int argc, const char **argv)
{
	return cmd_$1(argc, argv);
}
EOF
