# Skill: 42 Norm Guardian

Use this before committing or declaring a phase complete.

## Check
- Files and functions are small and readable.
- Names are clear and consistent.
- No C99 syntax.
- Headers have guards and no unnecessary includes.
- Makefile targets exist and do not relink unnecessarily.
- No forbidden functions.
- No mutable globals.
- README can explain blocking cases and synchronization.

## Defense mindset
Every non-trivial line must answer: what invariant does this preserve, and what failure would happen without it?
