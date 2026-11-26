# Documentation Guidelines

## Scope and Boundaries

- Edit existing files in place; do not rewrite them from scratch.
- Before editing, inspect `Documentation/index.rst` and sibling files under the same directory to establish document boundaries.
- Enforce single-source-of-truth scope: when referencing a topic owned by another document, keep it strictly high-level (omit implementation details and internal sub-targets) and link to the owning document with `:doc:` or `:ref:`.
- Preserve existing section hierarchy, title casing, definition lists, code-block examples, and `:ref:`/`:doc:` links. Modify only where the code has diverged, and add subsections only for new functionality.
- Cover all platforms handled in the code (e.g., Linux and Darwin/macOS); do not omit non-Linux branches.
- Match the document's abstraction level: omit internal plumbing (guard/dispatch variables, scratch/state files such as `.dump` or `.worktree-*`) unless they form an explicit cross-stage interface.
- Keep abstract examples as-is instead of hardcoding current repository values.

## Style and RST Formatting

- Write in a plain, dry, declarative style like Linux `Documentation/kbuild/`. State mechanics directly; prefer declarative statements over imperative instructions, and avoid rhetorical emphasis or filler transitions.
- Use parallel grammatical phrasing across sibling list and definition-list entries.
- End lead-in sentences that introduce a list or code block with a colon (`:`).
- Refer to external commands in prose using plain manpage notation (e.g., `make(1)`), not literal markup.
- Use ``literal`` markup for specific command invocations (e.g., ``make bootstrap``), paths, targets, variables, symbols, and flags.
- Use uppercase for macro, environment, and Makefile variable placeholders (e.g., ``NAME=y``); keep mathematical or descriptive placeholders lowercase (e.g., ``major * 1000 + minor``).
- Do not use `**bold**` in prose, and avoid gratuitous `.. note::`, `.. tip::`, or `.. important::` directives.
- Preserve tab indentation in definition lists and code blocks.
- Keep RST heading underline lengths identical to their titles.
- Hard-wrap prose at 80 columns: only break a line when it would exceed 80 columns.
