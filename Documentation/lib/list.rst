====
list
====

``list`` provides circular, doubly linked lists using embedded ``list_head``
structures.

The list head links to the first and last entries. In an empty list, both
links point to the head itself.

Entries are linked through a ``list_head`` member of their containing structure.

Initialization
--------------

``LIST_HEAD(name)``
	Declare and initialize an empty list head.

``LIST_HEAD_INIT(name)``
	Initialize a list head at declaration.

``list_head_init(head)``
	Initialize an existing list head.

Functions
---------

``list_add(new, head)``
	Insert an entry after the ``head``.

``list_add_tail(new, head)``
	Insert an entry before the ``head``.

``list_del(entry)``
	Remove an entry from its list.

	The entry's next and previous pointers are poisoned.

``list_is_first(entry, head)``
	Return nonzero if the entry is the first entry in the list.

``list_is_last(entry, head)``
	Return nonzero if the entry is the last entry in the list.

``list_is_head(entry, head)``
	Return nonzero if the entry is the list head.

``list_is_empty(head)``
	Return nonzero if the list is empty.

Entry access
------------

``list_entry(list, type, memb)``
	Return the containing structure from a ``list_head`` pointer.

	``type`` is the containing structure type. ``memb`` is the name of
	its ``list_head`` member.

``list_first_entry(head, type, memb)``
	Return the first entry in the list.

	The list must not be empty.

``list_last_entry(head, type, memb)``
	Return the last entry in the list.

	The list must not be empty.

``list_next_entry(pos, memb)``
	Return the entry following ``pos``.

``list_entry_is_head(entry, head, memb)``
	Return nonzero if the entry refers to the list head.

Iteration
---------

``list_foreach(pos, head)``
	Iterate over ``list_head`` entries from first to last.

``list_foreach_entry(pos, head, memb)``
	Iterate over containing structures from first to last.

``list_foreach_entry_safe(pos, tmp, head, memb)``
	Iterate over containing structures while allowing removal of the
	current entry.

	``tmp`` stores the next entry during iteration.

Mutation validation
-------------------

When ``CONFIG_VALIDATE_LIST_MUTATION`` is enabled, insertion and deletion
check neighboring links for corruption.

Invalid mutations invoke a non-returning failure handler.
