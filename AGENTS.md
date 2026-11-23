# Code

- Target Linux only.
- Put platform-specific code under `systemd/`.
- Don't call `die*()` functions in daemon code.
- Don't add device-enabled checks when modifying `lib/device.c`.

# Commit

- Follow the commit subject style used in this repository's history.
- Identify the primary purpose of the changes before committing.
- Don't use "and" in the commit subject.
- Write a commit subject only. Don't include a body.
