# SPDX-License-Identifier: GPL-3.0-or-later

sphinx-build ?= sphinx-build
sphinx-autobuild ?= sphinx-autobuild

sphinx-build += --conf-dir sphinx
sphinx-autobuild += --conf-dir sphinx

.PHONY: live html man info help

live:
	$(sphinx-autobuild) --builder html . build/html

html man info help:
	$(sphinx-build) --builder $@ . build/$@
