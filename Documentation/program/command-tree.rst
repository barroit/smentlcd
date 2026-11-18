.. SPDX-License-Identifier: GPL-3.0-or-later

============
Command Tree
============

The daemon and controller organize their commands into command trees. Each tree
has a root command, which may have multiple subcommands.

Each command may invoke a subcommand, which becomes the current command and may
invoke further subcommands.

Command Hierarchy
=================

For example, the controller has the following command tree:

.. code-block:: text

	smentlcdctl
	├── dump
	│   ├── file
	│   ├── stdin
	│   │   ├── follow
	│   │   └── once
	│   └── stdout
	├── help
	├── play
	├── stat
	└── version

Each command can invoke only its direct subcommands. For example, executing
*smentlcdctl dump stdin follow* involves three command invocations:

- The *root* command invokes *dump*.
- The *dump* command invokes *stdin*.
- The *stdin* command invokes *follow*.

Command Entry Points
====================

Command entry points accept argc and argv. The final command must terminate the
process rather than return to its caller.

The function name is derived from the command path. For example:

ctl/main.c
	Defines ``cmd_main()``, the root command.

ctl/main/dump.c
	Defines ``cmd_main_dump()``, the *dump* command.

ctl/main/dump/stdin.c
	Defines ``cmd_dump_stdin()``, the *dump stdin* command.

ctl/main/dump/stdin/follow.c
	Defines ``cmd_dump_stdin_follow()``, the *dump stdin follow* command.

Each subcommand also defines a ``help_*`` string with the same suffix as its
command function. This string describes the subcommand's brief usage in its
parent's command table.

Command Execution
=================

A command with subcommands uses ``pa_parse_args()`` to identify the requested
subcommand.

.. code-block:: c

	pa_command_fn cmd;
	struct pa_opt opts[] = {
		CMD_MAIN_TABLE(&cmd),
		PA_OPT_END(),
	};

	argc = pa_parse_args(argc, argv, opts, usage, 0);
	cmd(argc, argv);
	cc_trap();

The root command uses ``cc_trap()`` to trap an unexpected return from the
invoked command.

``CMD_MAIN_TABLE(v)`` contains the root command's direct subcommands.
``pa_parse_args()`` selects a command and stores its function in ``v``. The
selected function is then invoked with the remaining arguments.

A command with subcommands can repeat this process using its own command table.
For example, ``dump`` uses ``CMD_DUMP_TABLE(v)``, while *dump stdin* uses
``CMD_DUMP_STDIN_TABLE(v)``.

A command without subcommands executes its own operation. It may also use
``pa_parse_args()`` to process options, flags and bare arguments. For example,
the *play* command accepts image format options and a filename.

See :doc:`../core-api/parse_argv` for argv parsing.

Command Help Message
====================

Each command may define its own usage text, which is passed to
``pa_parse_args()``. For example, the *play* command defines:

.. code-block:: c

	static const char *usage[] = {
		"smentlcdctl play [<options>] <filename>",
		NULL,
	};

The command's ``help_*`` string provides a short description for its parent
command. For example:

.. code-block:: c

	const char *help_main_play = "play the image";

See :doc:`../build-system/command-generation` for the generation of command
tables and entry sources.
