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

# Python

Use single quote for string literals.
Use double quotes for f-strings.
Use from `X` import `Y`, `Z`, ... instead of import `X`.
Don't use `with` statement.
Don't use `Path` library.
Don't use `os.path.join`, format the path with f-string.
Don't close any file streams.
Don't pass `exist_ok=True` to `os.makedirs`.
Don't nest function calls. Assign the result of the inner call to a variable before passing it to another function.
Don't introduce variables midway through a block. Declare variables at the beginning of their enclosing block (script, function, loop, or conditional branch).

Make the smallest possible change to the existing code:
- Avoid clever Python idioms, tricks, or workarounds when a straightforward change is sufficient.
- Modify only the code necessary to fulfill the request. Do not change unrelated code.
- Prefer simple, direct conditions over refactoring or alternative algorithms.
- Preserve the existing structure and control flow as much as possible.
- Do not introduce additional loops, comprehensions, data structures, or helper functions if they are not strictly required.
