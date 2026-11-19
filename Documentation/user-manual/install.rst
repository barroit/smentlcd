.. SPDX-License-Identifier: GPL-3.0-or-later

====================
Program Installation
====================

This document describes how to build and install smentlcd on Ubuntu 26.04.

Prerequisites
=============

Install the required packages:

.. code-block:: console

	sudo apt install autoconf libtool gcc make cmake \
			 libudev-dev libsystemd-dev

Obtaining the Source
====================

Create a directory for the repository:

.. code-block:: console

	cd
	mkdir git
	cd git

Clone the repository via SSH:

.. code-block:: console

	git clone git@github.com:barroit/smentlcd.git

Alternatively, use HTTPS:

.. code-block:: console

	git clone https://github.com/barroit/smentlcd.git

Enter the repository:

.. code-block:: console

	cd smentlcd

Initialize submodules, for SSH access, use:

.. code-block:: console

	git submodule update --init

For HTTPS access, use:

.. code-block:: console

	git -c url.https://github.com/.insteadOf=git@github.com: \
	    -c url.https://gitlab.com/.insteadOf=git@gitlab.com: \
	    submodule update --init

Building
========

Build the bundled libraries:

.. code-block:: console

	./scripts/build-libdeflate.sh
	./scripts/build-libusb.sh

Apply the patches:

.. code-block:: console

	cat patch/* | patch -p1

.. warning::

	Do not skip this step. The build will succeed without these patches, but
	produce an incorrect binary.

Generate the default configuration and build the program:

.. code-block:: console

	make alldefconfig
	make -j

Deployment
==========

Install the program to $HOME/.local/bin:

.. code-block:: console

	make install

.. note::

	If the device is already plugged in, unplug and replug it after
	installation.

Add the current user to the smentlcd group created by *install* target:

.. code-block:: console

	sudo usermod -aG smentlcd $USER

Enable and start the user socket:

.. code-block:: console

	systemctl --user enable --now smentlcd.socket

Verification
============

Add $HOME/.local/bin to PATH so the shell can find the installed smentlcdctl
executable:

.. code-block:: console

	export PATH=$HOME/.local/bin:$PATH

Check the device status:

.. code-block:: console

	smentlcdctl stat
