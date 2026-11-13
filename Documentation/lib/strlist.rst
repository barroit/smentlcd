.. SPDX-License-Identifier: GPL-3.0-or-later

=========
strlist.h
=========

``strlist`` provides string lists with configurable storage and ownership.

Storage Modes
-------------

Each list uses one of three storage modes:

``SL_USE_CP``
	Store independent copies of strings.

``SL_USE_REF``
	Store references to strings without copying them.

``SL_USE_SB``
	Store strings in reusable ``strbuf`` buffers.

``sl_init()`` defaults to SL_USE_CP if no storage mode is specified. Only one
storage mode can be used at a time.

Other behavior flags may be combined with the selected storage mode, subject
to additional constraints enforced by assertions in ``lib/strlist.c``.

Ownership and Lifetime
----------------------

``sl_pop()`` removes the first string from the list.

With SL_USE_CP, the caller owns the returned string and must free it.

With SL_USE_REF, the returned string belongs to the caller's original storage.

With SL_USE_SB, the returned string belongs to the list. Subsequent
``sl_push()`` or ``sl_push_back()`` calls may invalidate the pointer.

SL_POP_DUP makes ``sl_pop()`` return an independent copy in reference and buffer
modes. The caller must free the copy.

List Operations
---------------

``sl_push()`` inserts at the front of the list, while ``sl_push_back()`` inserts
at the back. ``sl_pop()`` removes from the front.

With SL_RET_LEN, push operations return the string length. Otherwise, they
return ``(size_t)-1``.

Line Wrapping
-------------

``sl_read_line()`` splits text into lines based on a maximum display width.

The wrapping logic handles UTF-8 characters, counts wide characters as two
columns, and breaks at whitespace or recognized punctuation where possible.

SL_USE_REF mode is not supported for line wrapping.

API Reference
-------------

.. kernel-doc:: /include/strlist.h
   :symbols: strlist

.. kernel-doc:: /include/strlist.h
   :module: strlist_init_macro
   :symbols: SL_INIT_CP, SL_INIT

.. kernel-doc:: /include/strlist.h
   :symbols: SL_USE_CP,
	     SL_USE_REF,
	     SL_USE_SB,
	     SL_RET_LEN,
	     SL_POP_DUP,

.. kernel-doc:: /include/strlist.h
   :symbols: sl_init,
	     sl_destroy,
	     sl_push,
	     sl_push_back,
	     sl_pop,
	     sl_read_line,
	     sl_to_argv,

