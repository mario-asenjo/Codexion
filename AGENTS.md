# Codexion Agent Operating Manual

This repository is a 42-style C/POSIX Threads project. Treat this file as the local coordination contract for Hermes, Codex, Claude Code, OpenCode, and any human reviewer.

## Prime directive
Build Codexion incrementally. Do not generate a complete opaque solution in one pass. For each phase: explain the design and invariants briefly, implement the smallest useful slice, compile, test, update project memory/checklists, then stop for confirmation.

## Mandatory session startup
Before modifying code, every agent must read:
1. `AGENTS.md`.
2. `MEMORY.md`.
3. `docs/SPEC_DISTILLED.md`.
4. `docs/EVALUATION_CHECKLIST.md`.
5. `docs/AI_USAGE_LOG.md`.
6. The relevant docs under `docs/` for the current phase.

Do not keep repo-local skill files. Use public Hermes skills plus this AGENTS contract:
- Load public `ponytail` for every coding, review, refactor, simplification, or architecture decision. Apply it strictly: delete speculative scaffolding before adding new code.
- Load public `github-operations-workflows` for branches, commits, pushes, PRs, CI, or GitHub authentication.
- Load public `software-delivery-workflows` for planning, debugging, TDD, verification, and pre-commit review.

Project-specific expert roles stay documented below as review lenses, not as files under a `skills/` directory.

## Hard constraints
- Final binary name: `codexion`.
- Build with `make` from repository root.
- Required flags: `-Wall -Wextra -Werror -pthread`.
- Language: C, strict C89-compatible style.
- No libft.
- No bonus unless explicitly requested.
- Source/header files live under `coders/`.
- Required args: `number_of_coders time_to_burnout time_to_compile time_to_debug time_to_refactor number_of_compiles_required dongle_cooldown scheduler`.
- `argc == 9`.
- Scheduler must be exactly `fifo` or `edf`.
- No mutable global variables for resources, scheduler, logging, monitor, or config.
- No implementation of the final simulator should be added until the current phase explicitly calls for it.

## Allowed functions
`pthread_create`, `pthread_join`, `pthread_mutex_init`, `pthread_mutex_lock`, `pthread_mutex_unlock`, `pthread_mutex_destroy`, `pthread_cond_init`, `pthread_cond_wait`, `pthread_cond_timedwait`, `pthread_cond_broadcast`, `pthread_cond_destroy`, `gettimeofday`, `usleep`, `write`, `malloc`, `free`, `printf`, `fprintf`, `strcmp`, `strlen`, `atoi`, `memset`.

## Forbidden functions and constructs
No `calloc`, `realloc`, `strtol`, `isdigit`, `perror`, `strerror`, `exit`, `pthread_detach`, `pthread_cond_signal`, semaphores, `nanosleep`, `clock_gettime`, `sleep`, `sprintf`, `snprintf`, `qsort`.
No C99-only constructs: no `//`, no declarations in `for`, no VLA, no `<stdbool.h>`, avoid `<stdint.h>` unless justified.

## Internal roles

### codexion-architect
Owns architecture, invariants, and deadlock-resistant design. Rejects designs that duplicate dongles, hide scheduler behavior, or cannot be explained in peer evaluation.

### c89-norm-guardian
Owns C89 compatibility, authorized functions, 42 Norm style, clean headers, `Makefile`, no relink, and no mutable globals.

### pthread-concurrency-reviewer
Owns mutex/condition-variable discipline, lock ordering, absence of data races, absence of deadlocks, and serialized logs.

### scheduler-heap-specialist
Owns the binary heap, FIFO/EDF comparators, stable ordering, tie-breakers, and recode readiness.

### timing-monitor-specialist
Owns `gettimeofday`, relative timestamps, precise sleeps, burnout detection within 10 ms, and low-busy-wait monitoring.

### ponytail-reviewer
Uses the public `ponytail` skill. Removes speculative scaffolding, duplication, and cleverness while preserving C89, Norm, authorized functions, and current phase boundaries.

### evaluation-red-team
Simulates the evaluator. Hunts zero-grade failures: warnings, forbidden functions, leaks, races, deadlocks, mixed logs, timing drift, edge cases, and recode friction.

### readme-defense-coach
Owns README, oral defense, blocking cases, synchronization explanation, AI usage transparency, and peer-evaluation readiness.

## Lock-order contract
Recommended order:
1. `state_lock`
2. `dongle.lock` if explicitly used
3. `log_lock`

Never acquire `state_lock` after `log_lock`. Prefer one arbitration lock (`state_lock`) for granting two dongles atomically; individual dongle locks are optional and must not undermine the global order.

## Local command convention: `/explain <concept>`
If the user writes `/explain <concept>` inside a chat, answer as a mentor: define the concept, connect it to Codexion, show a small C/POSIX example or pseudocode if useful, list common traps, and finish with a peer-evaluation explanation the user can say out loud.

## Required workflow before finalizing code phases
- `make re` must be clean before any code phase is considered done.
- Run at least one small executable test after compiling.
- Update `MEMORY.md` with durable project decisions.
- Update `docs/AI_USAGE_LOG.md` when AI shaped an architectural or code decision.
- Check `docs/EVALUATION_CHECKLIST.md` for newly satisfied or risky items.
