.. SPDX-License-Identifier: GPL-3.0-or-later

============
parse_argv.h
============

``parse_argv`` parses command line arguments in-place.

The parser consumes known options and flags, and may select a command.
Remaining arguments are written back to ``argv`` and terminated with ``NULL``.

The caller owns all target storage. Initialize flag, count, number, string,
pick, and command mode targets before calling ``pa_parse_args()``. The parser
updates target objects but does not automatically initialize them.

Argument Syntax
---------------

An option begins with ``--``::

	--name
	--name=value
	--no-name

A flag begins with a single ``-``::

	-x
	-abc
	-xvalue

In ``-abc``, ``a``, ``b``, and ``c`` are processed individually unless a flag
takes a value. A value-taking flag consumes the rest of the string.

A bare argument does not begin with ``-``::

	file
	add
	origin/main

Parsing Flow
------------

The parser walks ``argv[1..]`` from left to right.

``argv[0]`` must contain the command name for the current parsing pass. The
parser appends it to the diagnostic path.

Example diagnostic path after nested parsing::

	prog add

Known options and flags are applied immediately. Bare arguments are retained,
rejected, or used for command lookup, depending on the parser flags and
command-selection state.

``pa_parse_args()`` reuses ``argv`` for output. Output arguments start at
``argv[0]``.

Command Selection
-----------------

When command entries exist, the parser matches eligible bare arguments against
command names.

The selected command remains in the output ``argv`` so the next parsing pass can
use it as ``argv[0]``.

Without ``PA_LAX_CMD``, the parser panics if no command is found.

With ``PA_LAX_CMD``, initialize ``*cmd_val`` to the default command's ``cmd_fn``
before parsing. If no command is selected, the parser inserts the default
command at ``argv[0]``.

``PA_LAX_CMD`` also enables ``PA_STOP_UNKNOWN`` so the default command can parse
its own options and flags later.

An unknown bare argument closes command lookup. Later options and flags are
still parsed, while subsequent bare arguments are returned as operands.

For example, with ``add`` as a valid command and ``status`` as the default::

	prog add -x
		=> command 'add' selected
		=> argv: add -x

	prog file add -x
		=> command lookup closed at 'file'
		=> argv: status file add -x

	prog file --verbose add
		=> command lookup closed at 'file'
		=> --verbose parsed
		=> argv: status file add

Entry Matching
--------------

An entry may provide an option form, a flag form, or both. A ``NULL`` name
disables the option form. A zero alias disables the flag form.

Option names may be abbreviated if the abbreviation uniquely identifies an
option. Ambiguous abbreviations are rejected.

For options, the text before ``=`` is used for matching. The text after ``=`` is
the value.

Option Negation
---------------

Unless ``PA_OPT_NO_NEG`` is set, options accept ``--no-name`` form.

For ordinary option names, this applies the unset operation. For names already
starting with ``no-``, the unset state is inverted.

The unset operation does not restore the caller's initial value. Its behavior
depends on the entry type.

``--no-`` is also a valid prefix and may be ambiguous when multiple negatable
options exist.

Values
------

Options accept values in either form::

	--name=value
	--name value

Flags accept values in either form::

	-nvalue
	-n value

For flag clusters, a value-taking flag consumes the remaining characters. For
example, ``-ofile`` assigns ``file`` to ``-o``.

For value-taking options and flags, the parser greedily consumes the next
argument if no value is attached (via ``=`` for options), even if it looks like
another option or flag.

With ``PA_OPT_LAX_ARG``, the value is optional. If no value is found, the parser
uses the entry default.

Examples::

	--mode --verbose
		=> mode's value is "--verbose"

	-m -v
		=> m's value is "-v"

Unknown Arguments
-----------------

With ``PA_STOP_UNKNOWN``, parsing stops at an unknown option or flag.

Bare ``-`` is treated as an unknown flag and returned in the output ``argv``.

For an unknown flag inside a cluster, previously recognized flags are applied,
and the remaining cluster is returned as a new flag argument.

For example, if ``-a`` is known and ``-b`` is unknown, ``-ab`` applies ``-a``
and returns ``-b``.

End of Options
--------------

``--`` stops option and flag parsing.

All remaining arguments are returned in the output ``argv``.

Help
----

``-h`` and ``--help`` print usage and exit.

Usage lines and entries are printed with command forms, flag forms, option
forms, value hints, and help text. Long text is wrapped.

API Reference
-------------

.. kernel-doc:: /include/parse_argv.h
