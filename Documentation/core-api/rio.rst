.. SPDX-License-Identifier: GPL-3.0-or-later

=====
rio.h
=====

``rio`` provides wrappers for I/O operations that retry when interrupted by a
signal.

Interruption Handling
---------------------

All functions retry the underlying system call when it fails with ``EINTR``.

Other errors are returned directly, with ``errno`` set by the underlying system
call.

API Reference
-------------

.. kernel-doc:: /include/rio.h
