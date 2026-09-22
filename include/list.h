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

struct list_head {
	struct list_head *next;
	struct list_head *prev;
};

#define LIST_HEAD(name) struct list_head name = LIST_HEAD_INIT(name)

#define LIST_HEAD_INIT(name) { &(name), &(name) }

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

static inline void list_add(struct list_head *new, struct list_head *head)
{
	__list_add(new, head, head->next);
}

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

static inline void list_del(struct list_head *entry)
{
	__list_del(entry);

	entry->next = POISON_ADDR_1;
	entry->prev = POISON_ADDR_2;
}

static inline int list_is_first(const struct list_head *entry,
				const struct list_head *head)
{
	return entry->prev == head;
}

static inline int list_is_last(const struct list_head *entry,
			       const struct list_head *head)
{
	return entry->next == head;
}

static inline int list_is_head(const struct list_head *entry,
			       const struct list_head *head)
{
	return entry == head;
}

static inline int list_is_empty(const struct list_head *head)
{
	return READ_ONCE(head->next) == head;
}

#define list_foreach_entry(pos, head, memb)			\
	for (pos = list_first_entry(head, typeof(*pos), memb);	\
	     !list_entry_is_head(pos, head, memb);		\
	     pos = list_next_entry(pos, memb))

#define list_foreach_entry_safe(pos, tmp, head, memb)		\
	for (pos = list_first_entry(head, typeof(*pos), memb),	\
		tmp = list_next_entry(pos, memb);		\
	     !list_entry_is_head(pos, head, memb);		\
	     pos = tmp, tmp = list_next_entry(tmp, memb))

#define list_first_entry(head, type, memb) \
	list_entry((head)->next, type, memb)

#define list_last_entry(head, type, memb) \
	list_entry((head)->prev, type, memb)

#define list_next_entry(pos, memb) \
	list_entry((pos)->memb.next, typeof(*(pos)), memb)

#define list_foreach(pos, head) \
	for (pos = (head)->next; !list_is_head(pos, head); pos = pos->next)

#define list_entry_is_head(entry, head, memb) \
	list_is_head(&(entry)->memb, head)

#define list_entry(list, type, memb) \
	container_of(list, type, memb)

#endif /* LIST_H */
