.. SPDX-License-Identifier: GPL-3.0-or-later

=====
log.h
=====

``log`` provides formatted logging, diagnostics, and error reporting.

Message Formatting
------------------

All logging functions use printf-style format strings.

Messages have the following format::

	[prefix ]message[; hint]

``prefix`` and ``hint`` are optional. Each message ends with a newline.

API Reference
-------------

.. kernel-doc:: /include/log.h
