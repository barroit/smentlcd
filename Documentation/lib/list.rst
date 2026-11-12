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
