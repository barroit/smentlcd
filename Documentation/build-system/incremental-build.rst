.. SPDX-License-Identifier: GPL-3.0-or-later

=================
Incremental Build
=================

The build system tracks header dependencies, configuration changes, compiler
flags, and linker flags to determine which targets need to be rebuilt.

Header Dependencies
===================

CFLAGS contains -MMD and -MP. During compilation, -MMD generates build/%.d with
dependencies on the source itself and non-system headers. -MP adds empty rules
for headers so removed headers do not cause missing target errors.

The Makefile generates build/%.d1 from build/%.d and includes the .d1 files for
objects in lib-obj-y, ctl-obj-y, and daemon-obj-y.

Configuration Dependencies
==========================

``scripts/fixconfig.sh`` scans the source and headers listed in build/%.d for
``CONFIG_*`` references. It converts each reference into a dependency under
include/config/. For example, CONFIG_ENABLE_DEBUG produces:

.. code-block:: make

	$(wildcard include/config/enable/debug.h)

The wildcard allows the dependency file to reference configuration headers
that may not exist.

This script adds these dependencies to build/%.d1 and removes
include/generated/config.h from the dependency list. This prevents changes to
the generated configuration header from rebuilding every object.

Kconfig updates files under include/config/ when configuration options change,
allowing make(1) to rebuild objects that reference the changed options.

Flag Changes
============

Three files record the compiler and linker flags:

build/.flags.cc
	Contains CFLAGS. Changes rebuild object files.

build/.flags.ld
	Contains LDFLAGS. Changes relink both executables.

build/.flags.ld-daemon
	Contains DAEMON_LDFLAGS. Changes relink the daemon executable.

Each file depends on .force, so its recipe runs whenever the file is needed.
The recipe uses ``mv_stale()`` to update the file only when the evaluated flags
differ from its contents or the file is missing. Otherwise, the file keeps its
timestamp, preventing unnecessary recompilation or relinking.
