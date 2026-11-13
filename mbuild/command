# SPDX-License-Identifier: GPL-3.0-or-later

include/%/d.h:
	mkdir -p $(@D)
	printf '%s\n' $| | sort | ./scripts/gen-d_h.sh $@ >$@

build/%/cmdtree: build/%/.commands scripts/build-cmdtree.py
	./scripts/build-cmdtree.py $*/main.c >$@

build/%/.commands: .force
	@mkdir -p $(@D)
	@trap 'rm -f .tmp-$$$$' EXIT && \
	find $* -type f -not -name '*_entry.c' | sort >.tmp-$$$$ && \
	$(call mv_stale,.tmp-$$$$,$(@),)
