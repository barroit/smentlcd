# SPDX-License-Identifier: GPL-3.0-or-later

name := smentlcd
version := 0.0.0

ifneq ($(filter extra-prereqs,$(.FEATURES)),extra-prereqs)
  $(error GNU Make >= 4.3 is required. Your Make version is $(MAKE_VERSION))
endif

MAKEFLAGS += -rR

build/$(name): build/$(name)d build/$(name)ctl

stage3_tagets := %.o %/d.h %/entrybuild/$(name)% build/t/unit/% \
		 dev_% .dev_% install% uninstall% miku
current_tagets := $(or $(MAKECMDGOALS),miku)

print_db := $(findstring p,$(firstword $(MAKEFLAGS)))
no_print_db := $(if $(print_db),,1)
on_stage3 := $(and $(no_print_db),$(filter $(stage3_tagets),$(current_tagets)))

define mv_stale
	test -f $(2) && cmp -s $(1) $(2) && test -z "$(3)" || \
	{ mv $(1) $(2) && touch $(2); }
endef

include scripts/Makefile.probe
include scripts/Makefile.kconfig

ifneq ($(on_stage3),)
  # We're compiling/linking.

  include build/probe/cc/features
  include build/probe/ld/features

  include include/config/auto.conf

  include build/daemon/cmdtree
  include build/ctl/cmdtree

  CC != cat build/probe/cc/program
  LD != cat build/probe/ld/id

  USE_GCC != test $$(cat build/probe/cc/id) = gcc && printf y
  USE_CLANG != test $$(cat build/probe/cc/id) = clang && printf 

  SERVICE != test $$(cat build/probe/host/id) = linux && printf systemd
endif

include scripts/Makefile.flags
include scripts/Makefile.install
include scripts/Makefile.develop

lib-obj-y += build/lib/atexit.o \
	     build/lib/list.o \
	     build/lib/log.o \
	     build/lib/parse_argv.o \
	     build/lib/rio.o \
	     build/lib/sanitizer.o \
	     build/lib/strbuf.o \
	     build/lib/strlist.o \
	     build/lib/strtox.o \
	     build/lib/strutil.o \
	     build/lib/unicode.o \
	     build/lib/unicode_width.o \
	     build/lib/xalloc.o

daemon-obj-y += build/lib/device.o \
		build/lib/log_nb.o

ifeq ($(SERVICE),systemd)
  daemon-obj-y += build/systemd/event.o \
		  build/systemd/ipc.o \
		  build/systemd/log_nb.o \
		  build/systemd/pcheck.o
endif

ifeq ($(CC_HAS_REALLOCARRAY),)
  lib-obj-y += build/lib/patch/reallocarray.o
endif

ifeq ($(CC_HAS_STRCHRNUL),)
  lib-obj-y += build/lib/patch/strchrnul.o
endif

include scripts/Makefile.daemon
include scripts/Makefile.ctl
include scripts/Makefile.command

ifneq ($(or $(print_db),$(CONFIG_ENABLE_TEST)),)
  include scripts/Makefile.unitest
  include scripts/Makefile.cmdtest
endif

-include $(lib-obj-y:.o=.d1)
-include $(daemon-obj-y:.o=.d1)

build/$(name)d: build/daemon/main/entry
	cp $< $@

build/$(name)ctl: build/ctl/main/entry
	cp $< $@

$(lib-obj-y):

build/%.o: %.c \
	   include/generated/build.h include/generated/features.h \
	   build/.flags.cc build/.flags.ld
	mkdir -p $(@D)
	$(CC) $(CFLAGS) $(addprefix -include ,$(filter include/generated/% \
						       include/ctl/% \
						       include/daemon/%,$^)) \
	      -c $< -o $@

%_entry.c: | %.c
	./scripts/gen-command-entry.sh $(*F) >$@

build/%.d1: build/%.d %.c
	@./scripts/fixconfig.sh $(shell grep .h: $< | tr -d : | \
					sed s,include/generated/config.h,,) \
				$*.c <$< >$@

lib/unicode_width.c:
	./scripts/gen-unicode_width_c.py >$@

.force:

.PHONY: clean distclean

distclean:
	rm -rf build include/daemon include/ctl include/config include/generated

clean:
	{ \
		find build/lib build/daemon build/ctl \
		     \( -name '*.o' -o -name '*.d' -o -name 'entry' \) \
		     -exec rm {} + ; \
		find include/daemon include/ctl -type f -exec rm {} + ; \
		find daemon ctl -name '*_entry.c' -exec rm {} + ; \
	} 2>/dev/null
	rm -f build/daemon/.commands build/daemon/cmdtree \
	      build/ctl/.commands build/ctl/cmdtree build/$(name)
