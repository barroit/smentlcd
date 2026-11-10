======
strtox
======

``strtox`` converts strings to integers with strict input and range validation.

The input must contain a valid integer. Leading or trailing whitespace, invalid
digits, and trailing characters are rejected.

Functions
---------

``strxtoull()``, ``strxtoul()``, ``strxtou()``
	Convert to ``unsigned long long``, ``unsigned long``, and
	``unsigned int``, respectively.

``strxtoll()``, ``strxtol()``, ``strxtos()``
	Convert to ``long long``, ``long``, and ``int``, respectively.

``strxtou64()``, ``strxtou32()``, ``strxtou16()``, ``strxtou8()``
	Convert to ``uint64_t``, ``uint32_t``, ``uint16_t``, and ``uint8_t``,
	respectively.

``strxtos64()``, ``strxtos32()``, ``strxtos16()``, ``strxtos8()``
	Convert to ``int64_t``, ``int32_t``, ``int16_t``, and ``int8_t``,
	respectively.

Conversion rules
----------------

All functions accept the same arguments:

``str``
	A NUL-terminated string to convert.

``base``
	The numeric base, from 2 to 36, or zero for automatic detection.

	When ``base`` is zero, the parser selects the base from the prefix:

	- ``0x`` or ``0X`` selects hexadecimal.
	- A leading ``0`` selects octal.
	- Otherwise, decimal is used.

	A leading ``0x`` or ``0X`` is skipped regardless of the specified
	base.

``res``
	A pointer to the converted result.

Digits are case-insensitive. An optional ``+`` or ``-`` sign is accepted.

Unsigned conversions accept negative values. The magnitude must fit the
target type before unsigned negation is applied.

Return value
------------

All functions return zero on success or -1 on failure.

On failure, ``errno`` is set to:

``EINVAL``
	The input is empty or contains invalid characters.

``ERANGE``
	The value exceeds the supported range or cannot fit the target type.

The ``res`` is left unchanged on failure.
