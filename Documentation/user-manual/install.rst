.. SPDX-License-Identifier: GPL-3.0-or-later

====================
Program Installation
====================

This document describes how to build and install smentlcd on Ubuntu 26.04.

.. caution::

	After installation:

	- Log out of the current session and log back in.
	- If the device is already plugged in, unplug and replug it.

Quick Installation
==================

Run all installation commands at once:

.. tabs::

	.. code-tab:: shell HTTPS

		sudo apt install autoconf libtool gcc make cmake \
				 libudev-dev libsystemd-dev && \
		git clone https://github.com/barroit/smentlcd.git && \
		cd smentlcd && \
		git -c url.https://github.com/.insteadOf=git@github.com: \
		    -c url.https://gitlab.com/.insteadOf=git@gitlab.com: \
		    submodule update --init && \
		./scripts/build-libdeflate.sh && \
		./scripts/build-libusb.sh && \
		cat patch/* | patch -p1 && \
		make alldefconfig && \
		make -j && \
		make install && \
		sudo usermod -aG smentlcd $USER && \
		systemctl --user enable --now smentlcd.socket

	.. code-tab:: shell SSH

		sudo apt install autoconf libtool gcc make cmake \
				 libudev-dev libsystemd-dev && \
		git clone git@github.com:barroit/smentlcd.git && \
		cd smentlcd && \
		git submodule update --init && \
		./scripts/build-libdeflate.sh && \
		./scripts/build-libusb.sh && \
		cat patch/* | patch -p1 && \
		make alldefconfig && \
		make -j && \
		make install && \
		sudo usermod -aG smentlcd $USER && \
		systemctl --user enable --now smentlcd.socket

Step-by-Step Installation
=========================

Install the required packages:

.. code-block:: shell

	sudo apt install autoconf libtool gcc make cmake \
			 libudev-dev libsystemd-dev

Clone the repository:

.. tabs::

	.. code-tab:: shell HTTPS

		git clone https://github.com/barroit/smentlcd.git
		cd smentlcd

	.. code-tab:: shell SSH

		git clone git@github.com:barroit/smentlcd.git
		cd smentlcd

Initialize submodules:

.. tabs::

	.. code-tab:: shell HTTPS

		git -c url.https://github.com/.insteadOf=git@github.com: \
		    -c url.https://gitlab.com/.insteadOf=git@gitlab.com: \
		    submodule update --init

	.. code-tab:: shell SSH

		git submodule update --init

Build the libraries:

.. code-block:: shell

	./scripts/build-libdeflate.sh
	./scripts/build-libusb.sh

Apply the patches:

.. code-block:: shell

	cat patch/* | patch -p1

.. warning::

	Do not skip this step. The build will succeed without these patches, but
	produce an incorrect binary.

Generate the release-ready configuration and build the daemon and controller:

.. code-block:: shell

	make alldefconfig
	make -j

Install the daemon and controller:

.. code-block:: shell

	make install

.. note::

	By default, the daemon and controller are installed in
	``$HOME/.local/bin``.

Add the current user to the ``smentlcd`` group created by the *install* target:

.. code-block:: shell

	sudo usermod -aG smentlcd $USER

Enable and start the user socket:

.. code-block:: shell

	systemctl --user enable --now smentlcd.socket

Verification
============

Add ``$HOME/.local/bin`` to PATH so the shell can find the installed smentlcdctl
executable:

.. code-block:: shell

	export PATH=$HOME/.local/bin:$PATH

Check the device status:

.. code-block:: shell

	smentlcdctl stat
