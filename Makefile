# SPDX-License-Identifier: GPL-3.0-or-later

name := smentlcd
version := 0.0.0

ifneq ($(filter extra-prereqs,$(.FEATURES)),extra-prereqs)
  $(error GNU Make >= 4.3 is required. Your Make version is $(MAKE_VERSION))
endif

MAKEFLAGS += -rR

build/$(name): build/$(name)d build/$(name)ctl

build_targets := build/.flags.% build/%.o include/%/cmdtab.h \
		 build/%/entry build/$(name)% build/t/unit/% \
		 dev_% .dev_% install% uninstall% miku
current_targets := $(or $(MAKECMDGOALS),miku)

print_db := $(findstring p,$(firstword $(MAKEFLAGS)))
no_print_db := $(if $(print_db),,1)
on_build := $(and $(no_print_db),$(filter $(build_targets),$(current_targets)))

define mv_stale_f
	test -f $(2) && cmp -s $(1) $(2) && test -z "$(3)" || { \
		mv $(1) $(2) && \
		touch $(2) && \
		$(if $(4),printf '%s >$(2)\n' '$(subst ','"'"',$(4))',:); \
	}
endef

define mv_stale_c
	trap 'rm -f .tmp-$$$$' EXIT; \
	($(1)) >.tmp-$$$$; \
	$(call mv_stale_f,.tmp-$$$$,$(2),$(3),$(1))
endef

include mbuild/probe
include mbuild/kconfig

ifneq ($(on_build),)
  # Make sure bootstrap is done.
  include build/.bootstrap

  include build/probe/cc/features
  include build/probe/ld/features

  include include/config/auto.conf

  include build/daemon/cmdtree
  include build/ctl/cmdtree

  CC := $(file <build/probe/cc/id)
  LD := $(file <build/probe/ld/id)

  USE_GCC := $(if $(filter gcc,$(CC)),y)
  USE_CLANG := $(if $(filter clang,$(CC)),y)

  SERVICE := $(if $(filter linux,$(file <build/probe/host/id)),systemd)
endif

include mbuild/flags
include mbuild/install
include mbuild/develop

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

ctl-obj-y += build/lib/image.o \
	     build/lib/packet.o

daemon-obj-y += build/lib/device.o \
		build/lib/log_nb.o \
		build/lib/playback.o

ifeq ($(SERVICE),systemd)
  ctl-obj-y += build/lib/stbi.o \
	       build/systemd/ipc_ctl.o

  daemon-obj-y += build/systemd/event.o \
		  build/systemd/ipc_daemon.o \
		  build/systemd/log_nb.o \
		  build/systemd/pcheck.o
endif

ifeq ($(CC_HAS_REALLOCARRAY),)
  lib-obj-y += build/lib/patch/reallocarray.o
endif

ifeq ($(CC_HAS_STRCHRNUL),)
  lib-obj-y += build/lib/patch/strchrnul.o
endif

include mbuild/executable
include mbuild/command

ifneq ($(or $(print_db),$(CONFIG_ENABLE_TEST)),)
  include mbuild/unitest
  include mbuild/cmdtest
endif

-include $(lib-obj-y:.o=.d1)
-include $(ctl-obj-y:.o=.d1)
-include $(daemon-obj-y:.o=.d1)

build/$(name)d: build/daemon/main/entry
	cp $< $@

build/$(name)ctl: build/ctl/main/entry
	cp $< $@

$(lib-obj-y):

build/%.o: %.c \
	   include/generated/build.h \
	   include/generated/features.h \
	   include/generated/repo.h \
	   build/.flags.cc
	mkdir -p $(@D)
	$(CC) $(CFLAGS) $(addprefix -include ,$(filter include/generated/% \
						       include/ctl/% \
						       include/daemon/%,$^)) \
	      -c $< -o $@

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
