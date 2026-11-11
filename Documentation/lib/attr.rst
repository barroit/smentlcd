======
attr.h
======

``attr`` provides macros for compiler attributes.

Optimization
------------

``attr_always_inline``
	Require the compiler to inline a function.

``attr_cold``
	Mark a function as unlikely to execute and optimize it for size.

Function properties
-------------------

``attr_noreturn``
	Indicate that a function does not return to its caller.

``attr_pure``
	Mark a function as having no observable side effects other than its
	return value.

``attr_malloc``
	Indicate that a function returns a pointer to memory that does not
	alias any other valid object.

Initialization and cleanup
--------------------------

``attr_constructor``
	Execute a function during program initialization.

``attr_cleanup(fn)``
	Call ``fn`` when a local variable goes out of scope.

	The function receives a pointer to the variable.

Diagnostics and analysis
------------------------

``attr_printf(fmt, va)``
	Enable compile-time checking of printf-style format strings and
	arguments.

	``fmt`` is the index of the format string argument. ``va`` is the
	index of the first argument to check.

``attr_warn_unused_result``
	Warn when a function's return value is ignored.

``attr_read_only(...)``
	Indicate that a pointer argument is read but not modified.

	The attribute is omitted if the compiler does not support it.

``attr_maybe_unused``
	Suppress warnings about unused declarations.

Symbols and sections
--------------------

``attr_always_used``
	Require the compiler to emit a function or variable even if it
	appears unused.

``attr_section(name)``
	Place a function or variable in the specified section.

``attr_weak``
	Declare a symbol as weak, allowing it to be overridden by a strong
	definition.

Instrumentation
---------------

``attr_no_asan``
	Disable AddressSanitizer instrumentation for a function.
