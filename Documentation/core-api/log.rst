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

Output Backend
--------------

.. c:var:: log_vwritef_fn log_vwritef

   Output handler for log_writef() and logging macros.

   Defaults to __log_vwritef(). Assign another handler to customize output.

API Reference
-------------

.. kernel-doc:: /include/log.h
