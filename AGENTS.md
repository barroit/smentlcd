# General

- Target Linux only.
- Put platform-specific code under `systemd/`.
- Don't call `die*()` functions in daemon code.
- Don't add device-enabled checks when modifying `lib/device.c`.

# Code Changes

Make the smallest possible change to the existing code:

- Modify only the code necessary to fulfill the request.
- Preserve the existing structure and control flow as much as possible.
- Prefer simple, direct conditions over refactoring or alternative algorithms.
- Avoid clever idioms, tricks, or workarounds when a straightforward change is sufficient.
- Don't introduce additional loops, comprehensions, data structures, or helper functions unless strictly necessary.

# Commit Messages

- Identify the primary purpose of the changes before committing.
- Follow the commit subject style used in this repository's history.
- Don't use "and" in the commit subject.
- Write a commit subject only. Don't include a body.

# Python

- Use single quotes for string literals.
- Use double quotes for f-strings.
- Use `from X import Y, Z, ...` instead of `import X`.
- Don't use `with` statements.
- Don't use `pathlib.Path`.
- Don't use `os.path.join`. Format paths with f-strings.
- Don't close any file streams.
- Don't pass `exist_ok=True` to `os.makedirs`.
- Don't nest function calls. Assign the result of the inner call to a variable before passing it to another function.
- Declare variables at the beginning of their enclosing block (script, function, loop, or conditional branch).
