==========
compiler.h
==========

``compiler`` provides compiler builtins, type checks, and compile-time
assertions.

Compiler builtins
-----------------

``cc_align_up(m, n)``
	Align ``m`` upward to a multiple of ``n``.

``cc_align_down(m, n)``
	Align ``m`` downward to a multiple of ``n``.

``cc_alignof(type)``
	Return the alignment requirement of a type or expression.

``cc_expect(x, val)``
	Provide the compiler with an expected value for ``x``.

``cc_offsetof(type, memb)``
	Return the byte offset of ``memb`` within ``type``.

``cc_popcount(x)``
	Return the number of set bits in ``x``.

``cc_trap()``
	Terminate execution using a compiler-generated trap.

``cc_types_compatible_p(a, b)``
	Return nonzero if the two types are compatible.

``cc_unreachable()``
	Inform the compiler that execution cannot reach this point.

Branch prediction
-----------------

``likely(x)``
	Indicate that ``x`` is expected to be true.

``unlikely(x)``
	Indicate that ``x`` is expected to be false.

Type checks
-----------

``type_is_same(a, b)``
	Return nonzero if the types of ``a`` and ``b`` are compatible.

``type_is_signed(x)``
	Return nonzero if the type of ``x`` is signed.

Compile-time assertions
-----------------------

``BUG_ON(x)``
	Produce a compilation error if ``x`` is true.

``BUG_ON_EXPR(x)``
	Produce a compilation error if ``x`` is true. Can be used within an
	expression.

``assert_same_type(a, b)``
	Require ``a`` and ``b`` to have compatible types.

``assert_diff_type(a, b)``
	Require ``a`` and ``b`` to have incompatible types.

``assert_array(x)``
	Require ``x`` to be an array.

``assert_pow2(x)``
	Require ``x`` to be a nonzero power of two.

``assert_ascii(x)``
	Require ``x`` to have its highest byte bit clear.

Container access
----------------

``container_of(x, type, memb)``
	Return a pointer to the containing structure from a pointer to
	its member.

	``type`` is the containing structure type. ``memb`` is the member
	name. The pointer type must match the member type or be ``void *``.
