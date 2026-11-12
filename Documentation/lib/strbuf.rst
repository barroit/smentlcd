.. SPDX-License-Identifier: GPL-3.0-or-later

========
strbuf.h
========

``strbuf`` provides mutable string buffers with automatic growth or fixed
capacity.

Storage Modes
-------------

``SB_ALLOC`` enables automatic allocation and growth.

``SB_PREALLOC`` uses a preallocated buffer with fixed capacity. Writes that
exceed the capacity are rejected without modifying the buffer.

Capacity includes space for the terminating NUL character.

``sb_destroy()`` and ``sb_reinit()`` free the buffer storage regardless of
the mode. Do not call them on buffers backed by stack or static storage.

Buffer Modification
-------------------

Write operations append to the buffer by default.

The ``_at`` variants write at a specified position, replacing existing content
and truncating the buffer after the written data.

The position must not exceed the current buffer length.

Write operations return the number of bytes written, or zero if a preallocated
buffer lacks sufficient capacity.

API Reference
-------------

.. kernel-doc:: /include/strbuf.h
   :symbols: SB_ALLOC, SB_PREALLOC, SB_INIT, SB_INIT_PREALLOC
   :module: strbuf_macro

.. kernel-doc:: /include/strbuf.h
   :symbols: strbuf,
	     sb_init,
	     sb_destroy,
	     sb_reinit,
	     sb_chop,
	     sb_trunc,
	     sb_write_ch,
	     sb_write_str,
	     sb_vwritef,
	     sb_writef,
	     sb_write_ch_at,
	     sb_write_str_at,
	     sb_vwritef_at,
	     sb_writef_at,
