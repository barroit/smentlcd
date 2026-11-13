# SPDX-License-Identifier: GPL-3.0-or-later

build/ctl/%/entry: $(lib-obj-y) $(ctl-obj-y) \
		   build/libdeflate.a
	mkdir -p $(@D)
	$(CC) $(filter %.o,$^) \
	      $(filter %.a,$^) \
	      $(LDFLAGS) \
	      -o $@

build/ctl/cmdtree:

build/ctl/.commands:
