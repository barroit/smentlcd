===
cpp
===

``cpp`` provides general-purpose C preprocessor utilities.

Macros
------

``ARGC(...)``
	Return the number of arguments, from 0 to 39.

``CALL(name, ...)``
	Append the argument count to ``name`` and invoke the resulting macro
	with the supplied arguments.

``NOOP(...)``
	Expand to a no-op statement that accepts up to five arguments without
	evaluating them.

``CONCAT(a, b)``
	Expand and concatenate ``a`` and ``b`` into a single preprocessing
	token.

``IS_ENABLED(x)``
	Return 1 if the Kconfig option ``x`` is enabled, or 0 otherwise.

Poison pointers
---------------

``POISON_ADDR_1`` through ``POISON_ADDR_6``
	Provide distinct non-NULL pointer values for marking invalidated
	pointers.
