.. SPDX-License-Identifier: GPL-3.0-or-later

========
log_nb.h
========

``log_nb`` provides a non-blocking logging backend using a ring buffer and a
worker thread.

Logging
-------

Messages are queued in a ring buffer and processed asynchronously by a worker.

When the ring buffer is full, the backend falls back to synchronous output.

Automatic commit notifies the worker when messages are queued. It is enabled
by default and can be controlled through ``log_nb_auto_commit()``.

Initialization
--------------

Call ``log_nb_init()`` to initialize the ring buffer, eventfd, and worker
thread. Then call ``log_nb_activate()`` to route logging output through the
backend.

``log_nb_deactivate()`` restores the default logging handler without stopping
the worker thread.

Worker failures disable the backend and restore the default logging handler.

Signal Handling
---------------

``log_nb_catch_signal()`` blocks SIGTERM and starts a thread that waits for the
signal. When received, the thread invokes the termination handler and exits the
process.

The termination handler wakes the worker, waits for it to drain pending
messages, and joins the worker thread.

``log_nb_restore_signal()`` cancels and joins the waiting thread. It does not
restore the previous signal mask.

Backend State
-------------

.. c:var:: int log_nb_enabled

   Nonzero when the non-blocking logging backend is active.

API Reference
-------------

.. kernel-doc:: /include/log_nb.h
