# SPDX-License-Identifier: GPL-3.0-or-later

build/ctl/%/entry: $(lib-obj-y) $(ctl-obj-y)
	mkdir -p $(@D)
	$(CC) $(LDFLAGS) \
	      $(filter %.o,$^) \
	      -o $@

build/ctl/cmdtree:

build/ctl/.commands:
