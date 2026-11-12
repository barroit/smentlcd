/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2026 Jiamu Sun <39@barroit.sh>
 */

#ifndef ATEXIT_H
#define ATEXIT_H

/**
 * typedef atexit_routine_t - Exit routine type.
 *
 * A function pointer to a routine that takes no arguments and
 * returns no value.
 */
typedef void (*atexit_routine_t)(void);

/**
 * atexit_push() - Push a routine onto the stack.
 * @fn: Routine to push.
 */
void atexit_push(atexit_routine_t fn);

/**
 * atexit_pop() - Remove the most recently pushed routine.
 *
 * The stack must not be empty.
 *
 * Return: The removed routine.
 */
atexit_routine_t atexit_pop(void);

/**
 * atexit_setup() - Register the exit handler.
 *
 * Register the exit handler using the standard library's atexit().
 */
void atexit_setup(void);

#endif /* ATEXIT_H */
