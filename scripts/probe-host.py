#!/usr/bin/python3
# SPDX-License-Identifier: GPL-3.0-or-later
#
# Copyright 2026 Jiamu Sun <39@barroit.sh>
#

from platform import freedesktop_os_release, mac_ver, machine, system
from sys import platform

kernel = system()
arch = machine()

print(platform, end='\t')

print(arch, end='\t')

if kernel == 'Darwin':
	info = mac_ver()

	print(f"macOS {info[0]}")
else:
	info = freedesktop_os_release()

	print(info['PRETTY_NAME'])
