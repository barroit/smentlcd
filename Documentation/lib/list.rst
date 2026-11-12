.. SPDX-License-Identifier: GPL-3.0-or-later

======
list.h
======

``list`` provides circular, doubly linked lists using embedded ``list_head``
structures.

The list head links to the first and last entries. In an empty list, both links
point to the head itself.

Entries are linked through a ``list_head`` member of their containing structure.

Mutation Validation
-------------------

When ``CONFIG_VALIDATE_LIST_MUTATION`` is enabled, insertion and deletion check
neighboring links for corruption.

Invalid mutations invoke a non-returning failure handler.

API Reference
-------------

.. kernel-doc:: /include/list.h
   :symbols: LIST_HEAD, LIST_HEAD_INIT
   :module: list_macro

.. kernel-doc:: /include/list.h
   :symbols: list_head,
	     list_head_init,
	     list_add,
	     list_add_tail,
	     list_del,
	     list_is_first,
	     list_is_last,
	     list_is_head,
	     list_is_empty,
	     list_foreach_entry,
	     list_foreach_entry_safe,
	     list_first_entry,
	     list_last_entry,
	     list_next_entry,
	     list_foreach,
	     list_entry_is_head,
	     list_entry,
