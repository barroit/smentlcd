.. SPDX-License-Identifier: GPL-3.0-or-later

========================
Three Stage Architecture
========================

This build system divides the entire build flow into 3 stages.

Bootstrap
=========

Bootstrap probes the build environment. At this stage, the build system detects
compiler metadata, linker metadata, repository specific constants, host
information, and an available UTF-8 locale. It stores the detected information
in dedicated files.

``mbuild/probe`` provides the bootstrap target and defines corresponding
targets.

Compiler / Linker Metadata
--------------------------

build/probe/{cc,ld}/program
	The program's absolute path.

build/probe/{cc,ld}/id
	The program's ID.

build/probe/{cc,ld}/version
	The program's version.

build/probe/{cc,ld}/name
	The program's human readable name.

build/probe/{cc,ld}/features
	The program's available features, make(1) readable.

These are probed by ``scripts/probe-cc*.sh`` and ``scripts/probe-ld*.sh``.

Compiler Feature Detection
^^^^^^^^^^^^^^^^^^^^^^^^^^

``scripts/probe-cc-features.sh`` defines the features to test. Tests come in two
forms:

.. code-block:: text

	probe_feature <<'EOF' &
	CC_HAS_GZ_ZLIB
	-g -gz=zlib
	EOF

Forms a flag-only test, and:

.. code-block:: text

	probe_feature <<'EOF' &
	CC_HAS_SSIZE_T
	-D_GNU_SOURCE

	#include <sys/types.h>

	ssize_t a = 0;
	EOF

Forms a program test.

The first line is the macro identifier. The second line contains the C compiler
flags, separated by whitespace. It's required for flag-only tests but is
optional for program tests. If the heredoc has no more lines, it becomes a
flag-only test; otherwise, it's a program test.

Linker Feature Detection
^^^^^^^^^^^^^^^^^^^^^^^^

``scripts/probe-ld-features.sh`` defines the features to test. The test is
formed in:

.. code-block:: text

	probe_feature <<'EOF' &
	LD_HAS_COMPRESS_DEBUG_SECTIONS_ZLIB
	-Wl,--compress-debug-sections=zlib
	EOF

Flags use the ``-Wl,<flag>`` format, separated by whitespace.

Repository Specific Constants
-----------------------------

build/probe/repo/name
	The project's name.

build/probe/repo/version
	The program's version.

These constants are defined in the Makefile:

.. code-block:: text

	name := example
	version := 8.3.1

Host Information
----------------

build/probe/host/id
	The host's ID.

build/probe/host/arch
	The host's architecture.

build/probe/host/name
	The host's human readable name.

These are obtained by python3.

UTF-8 Locale
------------

build/probe/utf8/locale
	The UTF-8 locale for LC_CTYPE.

This acts as an LC_CTYPE fallback source for uc_force_utf8_ctype(), see
:ref:`core-api/unicode:Locale Initialization`. It can be configured through
Kconfig.

Configuration
=============

At this stage, the build system runs Kconfig to generate build configurations.

Configuration Targets
---------------------

``mbuild/kconfig`` defines the ``alldefconfig`` and ``menuconfig`` targets.

alldefconfig
	This target generates the config file with default values. These values
	are ready for a release build.

menuconfig
	This target starts the TUI menu for changing the configuration.

Configuration Files
-------------------

These targets produce the following files:

- build/.config
	The kconfig state file.

- include/config/auto.conf
	The config file in a format readable by make(1).

- include/config/\*.h
	The dependency files used in incremental build.

- include/generated/config.h
	The header file that contains the config dump. It's injected into the
	source file via ``-include include/generated/config.h``.

Menu entries are defined in ``kconfig/*``.

Building
========

This stage is activated for build, development, and installation targets.

The build system loads the probed compiler and linker features, Kconfig
generated ``auto.conf``, and the daemon and controller command trees. These
inputs determine the compilation and linking flags, object selection, and
platform specific dependencies.

Executable Targets
------------------

- build/smentlcdd
	Build the daemon executable.

- build/smentlcdctl
	Build the controller executable.

- build/smentlcd
	Build both executables. This is the default target when the make(1) is
	invoked without any target.

Development Targets
-------------------

- dev_setup_unit
	Set up and start the daemon service.

- dev_teardown_unit
	Stop and tear down the daemon service.

- dev_monitor
	Show and follow the daemon logs.

Installation Targets
--------------------

- install
	Install the program and platform integration.

- uninstall
	Uninstall the program and platform integration.
