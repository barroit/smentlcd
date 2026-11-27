# SPDX-License-Identifier: GPL-3.0-or-later

name := smentlcd
version := 0.0.0

ifneq ($(filter extra-prereqs,$(.FEATURES)),extra-prereqs)
  $(error GNU Make >= 4.3 is required. Your Make version is $(MAKE_VERSION))
endif

MAKEFLAGS += -rR

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

build/$(name):

.force:

include mbuild/bootstrap
include mbuild/kconfig

ifneq ($(on_build),)
  # Make sure bootstrap is done.
  include build/.bootstrap

  include build/probe/cc/features
  include build/probe/ld/features

  include include/config/auto.conf

  include build/ctl/.cmdtree
  include build/daemon/.cmdtree

  CC := $(file <build/probe/cc/id)
  LD := $(file <build/probe/ld/id)

  USE_GCC := $(if $(filter gcc,$(CC)),y)
  USE_CLANG := $(if $(filter clang,$(CC)),y)

  SERVICE := $(if $(filter linux,$(file <build/probe/host/id)),systemd)
endif

include mbuild/flags
include mbuild/compile
include mbuild/command
include mbuild/executable

include mbuild/install
include mbuild/develop

ifneq ($(or $(print_db),$(CONFIG_ENABLE_TEST)),)
  include mbuild/testunit
  include mbuild/testcmd
endif

include mbuild/clean
