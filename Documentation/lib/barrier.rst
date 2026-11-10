=======
barrier
=======

``barrier`` provides atomic memory access and synchronization primitives using
compiler's ``__atomic`` builtins.

Macros
------

``READ_ONCE(x)``
	Atomically read ``x`` with relaxed memory ordering.

``WRITE_ONCE(x, val)``
	Atomically write ``val`` to ``x`` with relaxed memory ordering.

``smp_load_acquire(x)``
	Atomically load from ``x`` with acquire memory ordering.

``smp_store_release(x, val)``
	Atomically store ``val`` to ``x`` with release memory ordering.

``smp_inc_return(x)``
	Atomically increment ``*x`` and return the new value.

``smp_dec_return(x)``
	Atomically decrement ``*x`` and return the new value.

``smp_add_return(x, val)``
	Atomically add ``val`` to ``*x`` and return the new value.

``smp_sub_return(x, val)``
	Atomically subtract ``val`` from ``*x`` and return the new value.

``smp_test_and_set(x)``
	Atomically set the flag at ``x`` and return its previous state.

``smp_clear(x)``
	Clear the flag at ``x`` with release memory ordering.

``smp_mb()``
	Issue a sequentially consistent memory fence.

Memory ordering
---------------

Relaxed operations provide atomic access without imposing inter-thread ordering
constraints.

Acquire ordering prevents later memory accesses from being reordered before the
acquire operation. Release ordering prevents earlier memory accesses from being
reordered after the release operation.

Atomic arithmetic and test-and-set operations use acquire-release ordering.

``smp_mb()`` provides a full memory barrier using sequentially consistent
ordering.

For more details, see the Linux kernel's ``Documentation/memory-barriers.txt``.
