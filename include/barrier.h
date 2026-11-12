/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 *
 * For details on how memory barriers work, take a look at the linux kernel's
 * Documentation/memory-barriers.txt. Although it's related to the kernel, the
 * essential mechanisms are interoperable.
 */

#ifndef BARRIER_H
#define BARRIER_H

/**
 * macro READ_ONCE - Atomically read an object with relaxed memory ordering.
 * @x: Object to read.
 *
 * Return: The value read from @x.
 */
#define READ_ONCE(x) __atomic_load_n(&(x), __ATOMIC_RELAXED)

/**
 * macro WRITE_ONCE - Atomically write an object with relaxed memory ordering.
 * @x: Object to write.
 * @val: Value to write.
 */
#define WRITE_ONCE(x, val) __atomic_store_n(&(x), val, __ATOMIC_RELAXED)

/**
 * macro smp_load_acquire - Atomically load with acquire memory ordering.
 * @x: Pointer to the object to read.
 *
 * Return: The value read from @x.
 */
#define smp_load_acquire(x) __atomic_load_n(x, __ATOMIC_ACQUIRE)

/**
 * macro smp_store_release - Atomically store with release memory ordering.
 * @x: Pointer to the object to write.
 * @val: Value to write.
 */
#define smp_store_release(x, val) __atomic_store_n(x, val, __ATOMIC_RELEASE)

/**
 * macro smp_inc_return - Atomically increment an object with acquire-release
 *			  ordering.
 * @x: Pointer to the object to increment.
 *
 * Return: The new value.
 */
#define smp_inc_return(x) __atomic_add_fetch(x, 1, __ATOMIC_ACQ_REL)

/**
 * macro smp_dec_return - Atomically decrement an object with acquire-release
 *			  ordering.
 * @x: Pointer to the object to decrement.
 *
 * Return: The new value.
 */
#define smp_dec_return(x) __atomic_sub_fetch(x, 1, __ATOMIC_ACQ_REL)

/**
 * macro smp_add_return - Atomically add a value with acquire-release ordering.
 * @x: Pointer to the object to modify.
 * @val: Value to add.
 *
 * Return: The new value.
 */
#define smp_add_return(x, val) __atomic_add_fetch(x, val, __ATOMIC_ACQ_REL)

/**
 * macro smp_sub_return - Atomically subtract a value with acquire-release
 *			  ordering.
 * @x: Pointer to the object to modify.
 * @val: Value to subtract.
 *
 * Return: The new value.
 */
#define smp_sub_return(x, val) __atomic_sub_fetch(x, val, __ATOMIC_ACQ_REL)

/**
 * macro smp_test_and_set - Atomically set a flag with acquire-release ordering.
 * @x: Pointer to the flag.
 *
 * Return: The previous state of the flag.
 */
#define smp_test_and_set(x) __atomic_test_and_set(x, __ATOMIC_ACQ_REL)

/**
 * macro smp_clear - Clear an atomic flag with release memory ordering.
 * @x: Pointer to the flag.
 */
#define smp_clear(x) __atomic_clear(x, __ATOMIC_RELEASE);

/**
 * macro smp_mb - Issue a sequentially consistent memory fence.
 */
#define smp_mb() __atomic_thread_fence(__ATOMIC_SEQ_CST)

#endif /* BARRIER_H */
