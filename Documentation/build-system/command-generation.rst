.. SPDX-License-Identifier: GPL-3.0-or-later

==================
Command Generation
==================

The build system generates command tables, entry sources, and dependencies from
the daemon and controller command trees.

``mbuild/command`` defines the generation rules, implemented by the
``scripts/cmd-*`` scripts.

Build Rules
===========

``scripts/cmd-tree.py`` generates rules for command compilation and linking.
They are stored in:

build/daemon/cmdtree
	The daemon command build rules.

build/ctl/cmdtree
	The controller command build rules.

Each file contains:

- Prerequisites for generated command tables.
- Dependencies between command objects and their generated headers.
- Dependencies for the corresponding .d1 files.
- Object lists for command entry executables.
- cmd-obj-y assignments.

The Makefile includes both generated files during the
:ref:`build-system/three-stage:Building` stage.

Source Tracking
---------------

``mbuild/command`` maintains two files:

build/daemon/.commands
	Records the command list in the daemon command tree.

build/ctl/.commands
	Records the command list in the controller command tree.

Those files are checked on each invocation of make(1) that activates the build
stage rules. The file is updated only when its contents change, preventing
unnecessary regeneration.

A change to build/%/.commands causes the corresponding build/%/cmdtree to be
regenerated.

Command Tables
==============

``scripts/cmd-tab.sh`` generates the command table headers. For example:

include/ctl/main/cmdtab.h
	Contains CMD_MAIN_TABLE(v).

include/ctl/main/dump/cmdtab.h
	Contains CMD_DUMP_TABLE(v).

include/ctl/main/dump/stdin/cmdtab.h
	Contains CMD_DUMP_STDIN_TABLE(v).

Each generated header contains:

- Declarations for the command entry points.
- Declarations for the corresponding help message strings.
- A ``CMD_*_TABLE(v)`` macro containing PA_OPT_GROUP and PA_OPT_CMD entries.

Commands are grouped with a leading PA_OPT_GROUP entry. The group description is
generated from the first line of README and the current command path.

The generated build rules add each header as a prerequisite of the corresponding
command object and its direct subcommand objects.

During compilation, the build/%.o rule injects the generated header through the
compiler's -include option.

Entry Sources
=============

``scripts/cmd-entry.sh`` generates a C entry source for each command. The
generated source defines ``main()`` and calls the corresponding command
entry point.

For example, ctl/main_entry.c contains:

.. code-block:: c

	int cmd_main(int, const char **);

	int main(int argc, const char **argv)
	{
		return cmd_main(argc, argv);
	}

The %_entry.c rule generates these sources from their command paths.

Each entry executable links its generated entry object and the objects belonging
to its command subtree. The root entry executables are used to produce the
daemon and controller binaries.
