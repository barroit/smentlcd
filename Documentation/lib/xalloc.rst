======
xalloc
======

``xalloc`` provides memory allocation wrappers that terminate the process on
failure.

The wrappers record the source file, line number, and function name for
diagnostics.

Macros
------

``xmalloc(size)``
	Allocate ``size`` bytes.

``xcalloc(nmemb, size)``
	Allocate zero-initialized memory for ``nmemb`` elements of ``size``
	bytes each.

``xrealloc(ptr, size)``
	Resize an allocation to ``size`` bytes.

``xreallocarray(ptr, nmemb, size)``
	Resize an allocation to hold ``nmemb`` elements of ``size`` bytes
	each.

``xstrdup(s)``
	Duplicate a NUL-terminated string.

``xmmap(addr, len, prot, flags, fildes, off)``
	Map memory using the same arguments as ``mmap()``.

``XREALLOCBUF(x, new, cap)``
	Resize the array ``x`` using ``xreallocarray()``.

	``new`` is the required number of elements. ``cap`` is updated using
	``next_buf_size()`` to determine the new capacity.

Failure handling
----------------

Allocation and mapping failures produce a diagnostic message and abort the
process.

``xcalloc()`` and ``xreallocarray()`` also check for allocation size overflow.

When ``CONFIG_WARN_LARGE_ALLOC`` is enabled, ``xmalloc()``, ``xcalloc()``,
``xrealloc()``, and ``xreallocarray()`` report allocations greater than or
equal to ``CONFIG_LARGE_ALLOC`` without aborting.
