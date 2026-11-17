.. SPDX-License-Identifier: GPL-3.0-or-later

===========
Build Graph
===========

The build graph uses objects defined in lib-obj-y, daemon-obj-y, and ctl-obj-y.
Each executable links its selected objects and required external libraries.

Object Selection
================

The Makefile defines three object lists:

lib-obj-y
	Objects linked into both the daemon and controller.

daemon-obj-y
	Objects linked into the daemon.

ctl-obj-y
	Objects linked into the controller.

The Makefile extends these lists according to SERVICE and compiler features. For
exmaple, if CC_HAS_REALLOCARRAY missing, build/lib/patch/reallocarray.o is
added to lib-obj-y.

Compilation
===========

The build/%.o rule compiles each C source into the corresponding object under
build/. For example, lib/log.c produces build/lib/log.o.

Each object depends on its source, include/generated/build.h,
include/generated/features.h, and build/.flags.cc.

The compiler also injects the kconfig generated include/generated/config.h into
every source file. The build/%.o rule doesn't depend on this header. See
:doc:`incremental-build` for configuration dependencies.

Generated command headers can add dependencies. Their generation is described
in :doc:`command-generation`.

Linking
=======

The executable dependencies are:

.. code-block:: text

	build/smentlcd
	├── build/smentlcdd
	│   └── build/daemon/main/entry
	│       ├── lib-obj-y
	│       ├── daemon-obj-y
	│       ├── build/libusb.a
	│       ├── build/.flags.ld
	│       └── build/.flags.ld-daemon
	└── build/smentlcdctl
	    └── build/ctl/main/entry
	        ├── lib-obj-y
	        ├── ctl-obj-y
	        ├── build/libdeflate.a
	        └── build/.flags.ld

build/daemon/main/entry links objects defined in lib-obj-y and daemon-obj-y plus
build/libusb.a, with LDFLAGS and DAEMON_LDFLAGS.

build/ctl/main/entry links objects defined in lib-obj-y and ctl-obj-y plus
build/libdeflate.a, with LDFLAGS.

The entry executables are copied to build/smentlcdd and build/smentlcdctl.

build/libusb.a and build/libdeflate.a are hard links to external libraries
built under libusb/ and libdeflate/.
