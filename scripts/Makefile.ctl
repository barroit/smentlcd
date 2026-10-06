# SPDX-License-Identifier: GPL-3.0-or-later

build/ctl/%/entry: $(lib-obj-y) $(ctl-obj-y) \
		   build/libdeflate.a
	mkdir -p $(@D)
	$(CC) $(LDFLAGS) \
	      $(filter %.o,$^) \
	      $(filter %.a,$^) \
	      -o $@

build/ctl/cmdtree:

build/ctl/.commands:
