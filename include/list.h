/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 *
 * Check the 1c97be677f72b3c3 commit in linux kernel for the rationale behind
 * using WRITE_ONCE().
 */

#ifndef LIST_H
#define LIST_H

#include "attr.h"
#include "barrier.h"
#include "compiler.h"
#include "cpp.h"

/**
 * struct list_head - Node in a circular, doubly linked list.
 * @next: Pointer to the next node.
 * @prev: Pointer to the previous node.
 */
struct list_head {
	struct list_head *next;
	struct list_head *prev;
};

/**
 * macro LIST_HEAD - Declare and initialize an empty list head.
 * @name: Name of the list head.
 */
#define LIST_HEAD(name) struct list_head name = LIST_HEAD_INIT(name)

/**
 * macro LIST_HEAD_INIT - Initialize a list head at declaration.
 * @name: Name of the list head.
 */
#define LIST_HEAD_INIT(name) { &(name), &(name) }

/**
 * list_head_init() - Initialize an existing list head.
 * @head: List head to initialize.
 */
static inline void list_head_init(struct list_head *head)
{
	head->next = head;
	head->prev = head;
}

void attr_cold attr_noreturn __list_die_add(const struct list_head *new,
					    const struct list_head *prev,
					    const struct list_head *next);

void attr_cold attr_noreturn __list_die_del(const struct list_head *entry);

static inline void __list_add(struct list_head *new,
			      struct list_head *prev,
			      struct list_head *next)
{
	if (IS_ENABLED(CONFIG_VALIDATE_LIST_MUTATION)) {
		if (unlikely(new == prev || new == next ||
			     next->prev != prev || prev->next != next))
			__list_die_add(new, prev, next);
	}

	next->prev = new;
	new->next = next;
	new->prev = prev;
	WRITE_ONCE(prev->next, new);
}

/**
 * list_add() - Insert an entry after the list head.
 * @new: Entry to insert.
 * @head: List head.
 */
static inline void list_add(struct list_head *new, struct list_head *head)
{
	__list_add(new, head, head->next);
}

/**
 * list_add_tail() - Insert an entry before the list head.
 * @new: Entry to insert.
 * @head: List head.
 */
static inline void list_add_tail(struct list_head *new, struct list_head *head)
{
	__list_add(new, head->prev, head);
}

static inline void __list_del(struct list_head *entry)
{
	struct list_head *prev = entry->prev;
	struct list_head *next = entry->next;

	if (IS_ENABLED(CONFIG_VALIDATE_LIST_MUTATION)) {
		if (unlikely(next->prev != entry || prev->next != entry))
			__list_die_del(entry);
	}

	next->prev = prev;
	WRITE_ONCE(prev->next, next);
}

/**
 * list_del() - Remove an entry from its list.
 * @entry: Entry to remove.
 *
 * The entry's next and previous pointers are poisoned.
 */
static inline void list_del(struct list_head *entry)
{
	__list_del(entry);

	entry->next = POISON_ADDR_1;
	entry->prev = POISON_ADDR_2;
}

/**
 * list_is_first() - Test whether an entry is first in the list.
 * @entry: Entry to check.
 * @head: List head.
 *
 * Return: Nonzero if first, zero otherwise.
 */
static inline int list_is_first(const struct list_head *entry,
				const struct list_head *head)
{
	return entry->prev == head;
}

/**
 * list_is_last() - Test whether an entry is last in the list.
 * @entry: Entry to check.
 * @head: List head.
 *
 * Return: Nonzero if last, zero otherwise.
 */
static inline int list_is_last(const struct list_head *entry,
			       const struct list_head *head)
{
	return entry->next == head;
}

/**
 * list_is_head() - Test whether an entry is the list head.
 * @entry: Entry to check.
 * @head: List head.
 *
 * Return: Nonzero if the entry is the head, zero otherwise.
 */
static inline int list_is_head(const struct list_head *entry,
			       const struct list_head *head)
{
	return entry == head;
}

/**
 * list_is_empty() - Test whether a list is empty.
 * @head: List head.
 *
 * Return: Nonzero if empty, zero otherwise.
 */
static inline int list_is_empty(const struct list_head *head)
{
	return READ_ONCE(head->next) == head;
}

/**
 * macro list_foreach_entry - Iterate over entries from first to last.
 * @pos: Cursor pointing to the containing structure.
 * @head: List head.
 * @memb: Name of the embedded list_head member.
 */
#define list_foreach_entry(pos, head, memb)			\
	for (pos = list_first_entry(head, typeof(*pos), memb);	\
	     !list_entry_is_head(pos, head, memb);		\
	     pos = list_next_entry(pos, memb))

/**
 * macro list_foreach_entry_safe - Iterate while allowing entry removal.
 * @pos: Cursor pointing to the containing structure.
 * @tmp: Cursor storing the next entry.
 * @head: List head.
 * @memb: Name of the embedded list_head member.
 */
#define list_foreach_entry_safe(pos, tmp, head, memb)		\
	for (pos = list_first_entry(head, typeof(*pos), memb),	\
		tmp = list_next_entry(pos, memb);		\
	     !list_entry_is_head(pos, head, memb);		\
	     pos = tmp, tmp = list_next_entry(tmp, memb))

/**
 * macro list_first_entry - Get the first entry in a list.
 * @head: List head.
 * @type: Type of the containing structure.
 * @memb: Name of the embedded list_head member.
 *
 * The list must not be empty.
 *
 * Return: Pointer to the first entry.
 */
#define list_first_entry(head, type, memb) \
	list_entry((head)->next, type, memb)

/**
 * macro list_last_entry - Get the last entry in a list.
 * @head: List head.
 * @type: Type of the containing structure.
 * @memb: Name of the embedded list_head member.
 *
 * The list must not be empty.
 *
 * Return: Pointer to the last entry.
 */
#define list_last_entry(head, type, memb) \
	list_entry((head)->prev, type, memb)

/**
 * macro list_next_entry - Get the entry following a given entry.
 * @pos: Current entry.
 * @memb: Name of the embedded list_head member.
 *
 * Return: Pointer to the next entry.
 */
#define list_next_entry(pos, memb) \
	list_entry((pos)->memb.next, typeof(*(pos)), memb)

/**
 * macro list_foreach - Iterate over list nodes from first to last.
 * @pos: Cursor pointing to a list_head node.
 * @head: List head.
 */
#define list_foreach(pos, head) \
	for (pos = (head)->next; !list_is_head(pos, head); pos = pos->next)

/**
 * macro list_entry_is_head - Test whether an entry refers to the list head.
 * @entry: Entry to check.
 * @head: List head.
 * @memb: Name of the embedded list_head member.
 *
 * Return: Nonzero if the entry refers to the head, zero otherwise.
 */
#define list_entry_is_head(entry, head, memb) \
	list_is_head(&(entry)->memb, head)

/**
 * macro list_entry - Get the containing structure from a list node.
 * @list: Pointer to the embedded list_head member.
 * @type: Type of the containing structure.
 * @memb: Name of the embedded list_head member.
 *
 * Return: Pointer to the containing structure.
 */
#define list_entry(list, type, memb) \
	container_of(list, type, memb)

#endif /* LIST_H */
