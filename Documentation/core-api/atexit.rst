.. SPDX-License-Identifier: GPL-3.0-or-later

========
atexit.h
========

``atexit`` manages routines to be called at normal process termination.

Routines are stored in a stack and processed in LIFO order. The most recently
pushed routine is the first to be called.

Initialization
--------------

Call ``atexit_setup()`` during initialization to register the exit handler with
the standard library's ``atexit()``.

Routine Execution
-----------------

When the program returns from ``main()`` or calls ``exit()``, The exit handler
repeatedly pops and calls routines until the stack is empty.

Routines pushed during exit handling are also processed.

API Reference
-------------

.. kernel-doc:: /include/atexit.h
