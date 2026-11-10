==========
parse_argv
==========

``parse_argv`` parses command line arguments in-place.

The parser consumes known options and flags. When command entries exist, the
parser may also select a command. Remaining arguments are written back to
``argv`` and terminated with ``NULL``.

The caller owns all target storage. Initialize flag, count, number, string,
pick, and command mode targets before calling ``pa_parse_args()``. The parser
updates target objects but does not initialize them.

Terms
-----

``option``
	Argument that starts with ``--``.

	Examples::

		--name
		--name=value
		--no-name

``flag``
	Argument beginning with a single ``-``.

	Examples::

		-x
		-abc
		-xvalue

	In ``-abc``, ``a``, ``b``, and ``c`` are flags. The parser handles them
	one by one unless a flag takes a value. A value-taking flag consumes the
	rest of the string as its value.

``bare argument``
	Argument that doesn't start with ``-``.

	Examples::

		file
		add
		origin/main

Basic model
-----------

The parser walks ``argv[1..]`` from left to right.

``argv[0]`` must contain the command name for the current parsing pass. The
parser appends it to the diagnostic path.

Example diagnostic path after nested parsing::

	prog add

Known options and flags are applied immediately. Bare arguments are stored in
the output ``argv``, rejected, or used for command lookup, depending on the
parser state.

``pa_parse_args()`` reuses ``argv`` for output. It returns the number of
arguments left for the caller. Output arguments start at ``argv[0]``.

Parser flags
------------

``PA_STOP_BARE``
	Stop parsing at the first bare argument.

	Do not use this with command entries.

``PA_LAX_CMD``
	Make command selection optional.

	Before parsing, set ``*cmd_val`` to the default command's ``cmd_fn``.
	If no command is selected, the parser inserts the default command at
	``argv[0]``.

	This flag also enables ``PA_STOP_UNKNOWN`` so the default command can
	parse its own options and flags later.

	An unknown bare argument closes command lookup. The parser still parses
	later options and flags. Later bare arguments are returned as operands.

	Example, with ``add`` as a valid command and ``status`` as the default
	command::

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

``PA_NO_BARE``
	Reject bare arguments.

``PA_STOP_UNKNOWN``
	Stop parsing when an unknown option or flag is seen.

	Bare ``-`` is an unknown flag. With ``PA_STOP_UNKNOWN``, it is stored in
	the output ``argv``.

	For an unknown flag inside a flag cluster, the parser reformats the
	remaining part as a flag argument and stores it in the output ``argv``.
	If ``-a`` is known and ``-b`` is unknown, ``-ab`` applies ``-a`` and
	stores ``-b`` in the output ``argv``.

Entry helpers
-------------

``PA_OPT_END()``
	Terminate a ``pa_opt`` array. Required for every ``pa_opt`` list.

``PA_OPT_CMD(name, cmd_val, cmd_fn, help)``
	Add a command entry.

``PA_OPT_FLAG(name, alias, val, mask, help)``
	Add a flag-mask entry.

	The target is ``uint64_t``. The set form ORs ``mask`` into the target.
	The unset form clears ``mask`` from the target.

``PA_OPT_COUNT(name, alias, val, help)``
	Add a counter entry.

	The target is ``int``. The set form increments the target. The unset
	form resets the target to zero.

``PA_OPT_NUMBER(name, alias, val, help)``
	Add a number entry.

	By default, the target is ``unsigned int``. Use ``PA_OPT_SIGNED`` for
	an ``int`` target. Use ``PA_OPT_LLONG`` for an ``unsigned long long``
	target. Use both ``PA_OPT_SIGNED`` and ``PA_OPT_LLONG`` for a
	``long long`` target.

``PA_OPT_STRING(name, alias, val, help)``
	Add a string entry.

	The target is ``const char *``. The set form sets the target to the
	value. The unset form sets the target to ``NULL``.

``PA_OPT_FILE(name, alias, val, help)``
	Add a string entry with the value hint set to ``path``.

``PA_OPT_CMDMODE(name, alias, val, mode, help)``
	Add a command mode entry.

	The target is ``int``. The parser sets the target to ``mode``. Command
	mode entries for the same target (same pointer) are mutually exclusive.

``PA_OPT_PICK(name, alias, val, options, help)``
	Add a pick entry.

	The target is ``const char *``. ``options`` is a NULL-terminated string
	array used for validation. The value must match one item in the array.

	With ``PA_OPT_LAX_ARG``, a missing value makes the parser set the
	target to the first item in the array.

Entry matching
--------------

An entry may provide an option form, a flag form, or both. A NULL ``name``
means no option form. A zero ``alias`` means no flag form.

Option names may be abbreviated. The parser accepts an abbreviation if it
uniquely identifies an option. If multiple options match, the parser reports
ambiguity.

``=`` belongs to option syntax. The text before ``=`` is used for matching.
The text after ``=`` is the value.

Negative options
----------------

Unless ``PA_OPT_NO_NEG`` is set, options accept ``--no-name``.

For normal option names, ``--no-name`` sets the unset state.

For option names already starting with ``no-``, the unset state is inverted.

Caution: The unset state does not restore target objects to the caller's
default values. It applies the unset operation defined by the entry helper.

``--no-`` is a valid prefix for negatable options. It may be ambiguous when
more than one negatable option exists.

Values
------

An option or flag takes a value unless it has ``PA_OPT_NO_ARG``.

Options accept both forms::

	--name=value
	--name value

Flags accept both forms::

	-nvalue
	-n value

For flag clusters, a value-taking flag consumes the rest of the string as
its value. If ``-o`` takes a value, ``-ofile`` assigns ``file`` to ``-o``.

``PA_OPT_LAX_ARG`` makes the value optional.

Optional values are consumed greedily. If the next ``argv`` item exists, the
parser consumes it as the value. Otherwise, the parser uses the entry default.

Examples::

	--mode --verbose
		=> mode value is "--verbose"

	-m -v
		=> m value is "-v"

Commands
--------

A command is a special bare argument.

When command entries exist, the parser tries to match an eligible bare
argument against command names. The command name remains in the output
``argv`` so the next parsing pass can use it as ``argv[0]``.

If command entries exist and ``PA_LAX_CMD`` is not set, the parser panics if
no command is found.

End of options
--------------

``--`` stops option and flag parsing.

Remaining arguments after ``--`` are stored in the output ``argv``.

Help
----

``-h`` and ``--help`` print usage and exit.

Usage lines and entries are printed. Entries show the command form, flag form,
option form, value hint, and help text. Long text is wrapped.

Return value
------------

``pa_parse_args()`` returns the number of arguments stored in the output
``argv``.

The parser writes output arguments to ``argv[0..ret - 1]`` and appends a
``NULL`` terminator.
