.. SPDX-License-Identifier: GPL-3.0-or-later

========
strtox.h
========

``strtox`` converts strings to integers with strict input and range validation.

Conversion Rules
----------------

The input must contain a valid integer. Leading or trailing whitespace, invalid
digits, and trailing characters are rejected.

The numeric base must be between 2 and 36, or zero for automatic detection.

When the base is zero, the parser selects the base from the prefix:

- ``0x`` or ``0X`` selects hexadecimal.
- A leading ``0`` selects octal.
- Otherwise, decimal is used.

Digits are case-insensitive. An optional ``+`` or ``-`` sign is accepted.

Unsigned conversions accept a leading ``-`` if the absolute value fits the
target type. The result wraps according to unsigned arithmetic. For example,
``-1`` becomes the maximum value of the target type.

Error Handling
--------------

All functions return zero on success or -1 on failure.

On failure, ``errno`` is set to:

``EINVAL``
	The input is empty or contains invalid characters.

``ERANGE``
	The value exceeds the supported range or cannot fit the target type.

The result is left unchanged on failure.

API Reference
-------------

.. kernel-doc:: /include/strtox.h
