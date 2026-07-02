# Skill: C89 Low-Level C

Use this when writing or reviewing Codexion C code.

## Rules
- Keep declarations at the beginning of blocks.
- Use `/* ... */` comments only.
- Do not use VLA, `stdbool.h`, declarations in `for`, or C99 syntax.
- Check every allocation and initialization path.
- Prefer explicit ownership: who allocates, who destroys, who frees.
- Keep headers minimal and guarded.

## Review checklist
- Function has one clear responsibility.
- No forbidden function appears.
- Error path releases what was already acquired.
- No hidden global state.
- Types are simple enough to explain.
- Code compiles with `-Wall -Wextra -Werror -pthread`.
