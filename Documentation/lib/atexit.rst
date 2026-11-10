======
atexit
======

``atexit`` manages routines to be called at normal process termination.

Routines are stored in a stack and processed in LIFO order. The most recently
pushed routine is the first to be called.

Call ``atexit_setup()`` to enable automatic execution at process exit.

Routine type
------------

``atexit_routine_t``
	A function pointer to a routine that takes no arguments and returns
	no value.

Functions
---------

``atexit_push(fn)``
	Push a routine onto the stack.

``atexit_pop()``
	Remove and return the most recently pushed routine.

	The stack must not be empty.

``atexit_setup()``
	Register the exit handler using the C library's ``atexit()``.

	The handler is called when the program returns from ``main()`` or
	calls ``exit()``.

Exit handling
-------------

The exit handler repeatedly pops and calls routines until the stack is empty.

Routines pushed during exit handling are also processed.
