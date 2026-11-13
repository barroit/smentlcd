.. SPDX-License-Identifier: GPL-3.0-or-later

=========
barrier.h
=========

``barrier`` provides atomic memory access and synchronization primitives using
the compiler's ``__atomic`` builtins.

Memory Ordering
---------------

Relaxed operations provide atomic access without imposing inter-thread ordering
constraints.

Acquire ordering prevents later memory accesses from being reordered before the
acquire operation. Release ordering prevents earlier memory accesses from being
reordered after the release operation.

Atomic arithmetic and test-and-set operations use acquire-release ordering.

``smp_mb()`` provides a full memory barrier using sequentially consistent
ordering.

For more details, see the Linux kernel's ``Documentation/memory-barriers.txt``.

API Reference
-------------

.. kernel-doc:: /include/barrier.h
