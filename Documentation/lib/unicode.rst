.. SPDX-License-Identifier: GPL-3.0-or-later

=========
unicode.h
=========

``unicode`` provides utilities for working with Unicode text.

UTF-8 Handling
--------------

The byte classification macros distinguish single-byte characters, multibyte
leading bytes, and continuation bytes.

``uc_mbslen()`` counts characters in a NUL-terminated UTF-8 string, while
``uc_mbtowc()`` decodes a single UTF-8 sequence.

These functions assume valid UTF-8 input and do not validate complete multibyte
sequences.

Locale Initialization
---------------------

``uc_force_utf8_ctype()`` attempts to configure ``LC_CTYPE`` using the following
sources, in order:

1. The ``LC_CTYPE`` environment variable.
2. ``CONFIG_UTF8_LOCALE``.
3. Common ``C.UTF-8`` locale variants.

The process terminates if no locale can be selected.

Character Width
---------------

``uc_iswide()`` uses a generated table of Unicode character ranges to identify
wide characters.

API Reference
-------------

.. kernel-doc:: /include/unicode.h
