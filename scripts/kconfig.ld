# SPDX-License-Identifier: GPL-3.0-or-later

SECTIONS {
	.kconfig (COPY) : {
		KEEP(build/.config(.data))
	}
}

INSERT AFTER .text;
