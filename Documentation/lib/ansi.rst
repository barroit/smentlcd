.. SPDX-License-Identifier: GPL-3.0-or-later

======
ansi.h
======

``ansi`` provides ANSI escape codes for terminal text formatting.

Formatting Codes
----------------

Formatting sequences begin with ``\033[`` and end with ``m``. Multiple codes
are separated by semicolons.

The following macros define formatting codes:

``ANSI_RESET``
	Reset all formatting.

``ANSI_BOLD``
	Enable bold or bright intensity.

``ANSI_BLACK`` through ``ANSI_WHITE``
	Select standard foreground colors (30–37).

``ANSI_BRIGHT_BLACK`` through ``ANSI_BRIGHT_WHITE``
	Select bright foreground colors (90–97).

``ANSI_BG_BLACK`` through ``ANSI_BG_WHITE``
	Select standard background colors (40–47).

Example::

	H("Hello", ANSI_BOLD, ANSI_RED)

This produces bold red text followed by a formatting reset.

API Reference
-------------

.. kernel-doc:: /include/ansi.h
