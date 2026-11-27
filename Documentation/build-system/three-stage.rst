.. SPDX-License-Identifier: GPL-3.0-or-later

========================
Three Stage Architecture
========================

This build system divides the entire build flow into 3 stages.

Bootstrap
=========

``make bootstrap`` records compiler and linker metadata, repository constants,
host information, and the current UTF-8 locale, if any, under ``build/probe/``.
``mbuild/bootstrap`` defines the probe rules. When the probes are complete, the
target creates ``build/.bootstrap``, which configuration and building require.

Toolchain Selection
-------------------

``scripts/probe-tools.sh`` tries ``CC`` and ``LD`` first if both are set. On
Darwin it next tries ``clang`` with ``ld64.lld``. On all hosts it then tries
``gcc`` with ``ld.bfd``, followed by ``clang`` with ``ld.lld``. It selects the
first pair that meets both requirements:

- The compiler is GCC 12 or later, or Clang 18 or later.
- The linker is BFD or LLD and can successfully link a test program using the
  selected compiler.

Compiler / Linker Metadata
--------------------------

``build/probe/{cc,ld}/program``
	The program path found by the probe.

``build/probe/{cc,ld}/id``
	The compiler ID (``gcc`` or ``clang``) or linker ID (``bfd`` or
	``lld``).

``build/probe/{cc,ld}/version``
	The version encoded as ``major * 1000 + minor``.

``build/probe/{cc,ld}/name``
	The program's human readable name.

``build/probe/{cc,ld}/features``
	One ``NAME=y`` assignment per successful feature probe, readable by
	make(1).

These are probed by ``scripts/probe-cc*.sh`` and ``scripts/probe-ld*.sh``.

Compiler Feature Detection
^^^^^^^^^^^^^^^^^^^^^^^^^^

``scripts/probe-cc-features.sh`` defines flag and program tests. A flag test
compiles an empty source file with the specified flags:

.. code-block:: shell

	probe_feature <<'EOF' &
	CC_HAS_GZ_ZLIB
	-g -gz=zlib
	EOF

A program test compiles the supplied source to assembly:

.. code-block:: shell

	probe_feature <<'EOF' &
	CC_HAS_SSIZE_T
	-D_GNU_SOURCE

	#include <sys/types.h>

	ssize_t a = 0;
	EOF

The first line is the macro identifier. The second line contains the compiler
flags separated by whitespace. Omit this line for a program test that needs no
flags. Both forms treat compiler warnings as errors.

Linker Feature Detection
^^^^^^^^^^^^^^^^^^^^^^^^

``scripts/probe-ld-features.sh`` uses the selected compiler and linker to link a
test object with the specified flags:

.. code-block:: shell

	probe_feature <<'EOF' &
	LD_HAS_COMPRESS_DEBUG_SECTIONS_ZLIB
	-Wl,--compress-debug-sections=zlib
	EOF

Flags use the ``-Wl,<flag>`` format, separated by whitespace.

Repository Specific Constants
-----------------------------

``build/probe/repo/name``
	The project's name.

``build/probe/repo/version``
	The program's version.

The top-level ``Makefile`` defines those constants:

.. code-block:: make

	name := example
	version := 8.3.1

Host Information
----------------

``build/probe/host/id``
	The host's platform ID, such as ``linux`` or ``darwin``.

``build/probe/host/arch``
	The host's architecture.

``build/probe/host/name``
	The distribution's ``PRETTY_NAME`` on Linux or ``macOS`` followed by the
	version on Darwin.

``scripts/probe-host.py`` obtains these values.

UTF-8 Locale
------------

``build/probe/utf8/locale``
	The current ``LC_CTYPE`` value if it uses UTF-8, or an empty file
	otherwise.

This value supplies the Kconfig default for ``CONFIG_UTF8_LOCALE``, which
``uc_force_utf8_ctype()`` uses as a fallback. See
:ref:`core-api/unicode:Locale Initialization`.

Configuration
=============

Kconfig reads the bootstrap results to set configuration defaults and determine
which compiler and linker options are available.

Configuration Targets
---------------------

``mbuild/kconfig`` defines the ``alldefconfig`` and ``menuconfig`` targets.

Both configuration targets require ``build/.bootstrap``.

``alldefconfig``
	Generate the configuration file with default values for a release build.

``menuconfig``
	Open the configuration menu. On exit, generate the build configuration
	files if ``build/.config`` exists.

Configuration Files
-------------------

Kconfig produces the following configuration and dependency files:

- ``build/.config``
	The Kconfig state file.

- ``include/config/auto.conf``
	The config file in a format readable by make(1).

- ``include/config/**/*.h``
	Per-option dependency headers for incremental builds. See
	:ref:`build-system/incremental-build:Configuration Dependencies`.

- ``include/generated/config.h``
	The header file that contains the config dump. It's injected into the
	source file via ``-include include/generated/config.h``.

The top-level ``Kconfig`` reads ``kconfig/compile`` and ``kconfig/runtime``.

Building
========

Compilation, linking, development, and installation targets activate this stage.
The building stage requires a generated configuration.

The build system reads the probed compiler and linker features,
``include/config/auto.conf``, and the generated command trees in
``build/daemon/.cmdtree`` and ``build/ctl/.cmdtree``. These inputs determine the
compilation and linking flags and object selection. See :doc:`build-graph` and
:doc:`command-generation`.

``mbuild/flags`` sets the compiler and linker flags. If ``CONFIG_EMBED_CONFIG``
is enabled, the linker embeds ``build/.config`` in ELF or Mach-O executables.
With ``CONFIG_ENABLE_TEST`` enabled, the build includes ``mbuild/testunit`` and
``mbuild/testcmd``. See :doc:`../test/index`.

Generated Headers
-----------------

``mbuild/compile`` defines the object compilation rules and generates two
headers from the bootstrap results:

``include/generated/build.h``
	Contains compiler and linker IDs and versions, repository constants, and
	host information. Generated by ``scripts/gen-headers.sh``.

``include/generated/features.h``
	Contains macros for the detected compiler and linker features. Generated
	by ``scripts/gen-features_h.sh``.

Executable Targets
------------------

``mbuild/executable`` defines the entry executable and symbolic link targets:

- ``build/smentlcdd``
	Create a symbolic link to the daemon executable at
	``build/daemon/main/entry``.

- ``build/smentlcdctl``
	Create a symbolic link to the controller executable at
	``build/ctl/main/entry``.

- ``build/smentlcd``
	Build both entry executables and create their symbolic links. This is
	the default target when make(1) is invoked without a target.

Development Targets
-------------------

- ``dev_setup_unit``
	Set up and start the daemon service for the current user.

- ``dev_teardown_unit``
	Stop and disable the daemon service for the current user.

- ``dev_monitor``
	Follow the daemon logs.

- ``dev_link_program``
	Build both executables and link them into ``$HOME/.local/bin``.

Installation Targets
--------------------

- ``install``
	Install the program and its platform integration.

- ``uninstall``
	Remove the program and its platform integration.

See :doc:`../user-manual/install`.
