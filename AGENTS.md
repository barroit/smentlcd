This repository targets Linux.
Put platform-specific code under `./systemd`.
Do not call die*() functions unless I ask.
Do not try to compile if the current host is macOS.
Do not check if device is enabled when patching lib/device.c.
