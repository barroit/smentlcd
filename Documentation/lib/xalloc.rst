.. SPDX-License-Identifier: GPL-3.0-or-later

========
xalloc.h
========

``xalloc`` provides memory allocation wrappers that terminate the process on
failure.

The wrappers record the source file, line number, and function name for
diagnostics.

Failure Handling
----------------

Allocation and mapping failures produce a diagnostic message and abort the
process.

``xcalloc()`` and ``xreallocarray()`` also check for allocation size overflow.

When ``CONFIG_WARN_LARGE_ALLOC`` is enabled, ``xmalloc()``, ``xcalloc()``,
``xrealloc()``, and ``xreallocarray()`` report allocations greater than or
equal to ``CONFIG_LARGE_ALLOC`` without aborting.

API Reference
-------------

.. kernel-doc:: /include/xalloc.h
