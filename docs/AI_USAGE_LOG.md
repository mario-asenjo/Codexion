# AI Usage Log

This project uses AI as a mentoring and review tool, not as a substitute for understanding.

## Phase 0
AI was used to:
- structure the repository bootstrap;
- distill the specification into checklists;
- propose a concurrency architecture candidate;
- document scheduler, timing, and evaluation risks;
- prepare a README template and defense-oriented notes.

No final simulator implementation was generated in Phase 0.

## Ongoing rule
For every later phase, record significant AI assistance here when it affects architecture, concurrency decisions, parser behavior, test strategy, or code review.

The student must understand and validate every line of code before delivery. If code is generated or reviewed with AI, it must still be compiled, tested, and orally defensible.

## Phase 1
AI was used to:
- create the feature-branch and PR-template workflow;
- draft the minimal C89 skeleton structure;
- choose a manual numeric parser because `strtol` and `isdigit` are forbidden;
- keep Phase 1 intentionally non-simulating so the final concurrency design is not hidden in a large opaque implementation;
- identify that the local Windows PATH lacks `cc`, then verify compilation with the available Qt LLVM clang override while preserving `CC = cc` in the Makefile.

The implemented code is limited to argument validation, base types, time helpers, and serialized logging support.

## Phase 2
AI was used to:
- implement a small fixed-capacity binary heap for scheduler requests;
- keep heap mechanics separate from FIFO/EDF policy;
- isolate the EDF equal-deadline tie-breaker in `cx_edf_tie_break` for recode;
- create temporary WSL-only heap verification instead of adding non-deliverable test files to the project tree.

No simulation threads, dongle arbitration, monitor, or cooldown behavior were implemented in this phase.

## Phase 3
AI was used to:
- add the simulation state ownership boundary in `cx_sim_init` and `cx_sim_destroy`;
- define cleanup order for heap, dongle mutexes, simulation mutexes/condition variable, and arrays;
- create temporary WSL-only init/destroy tests for 1, 5, and 200 coders;
- use AddressSanitizer leak detection because Valgrind is not installed in the current WSL environment and passwordless sudo is unavailable.

No coder threads, monitor thread, scheduler arbitration, dongle acquisition, cooldown behavior, or logging lifecycle were implemented in this phase.

## Norminette cleanup
AI was used to split existing files only where Norminette required it. No behavior change was intended: heap pop/order helpers, usage printing, and simulation setup helpers moved into smaller files.

## Repository skill cleanup
AI was used to:
- replace repo-local skill markdown files with AGENTS.md instructions that point agents to public Hermes skills;
- remove the repo-created Ponytail optimizer file in favor of the public `ponytail` skill;
- apply a Ponytail review to delete unused future simulation structs from the current header;
- clarify that `docs/ARCHITECTURE.md` structure examples are design notes, not pre-added scaffolding.

No simulator behavior was introduced by this cleanup.

## Phase 4
AI was used to wire the smallest useful coder lifecycle: create/join coder threads, heap-ordered compile requests, atomic two-dongle ownership under `state_lock`, cooldown on release, and compile/debug/refactor logs.

Deliberate simplification: burnout detection and monitor-thread stop coordination remain Phase 5; Phase 4 targets viable completion cases.

## Phase 5
AI was used to add the smallest monitor slice: one monitor thread, deadline scan under `state_lock`, coordinated stop/broadcast on burnout, one allowed burnout log after stop, and suppression of normal logs once stopped.

Deliberate simplification: monitor timing uses bounded 1 ms sleeps instead of a larger timed-wait abstraction; upgrade only if later race/timing audits prove it necessary.

## Phase 6
AI was used to define and run the smallest scheduler validation set: one-coder burnout, forced burnout stop coordination, viable FIFO, viable EDF, and one scenario where FIFO/EDF compile sequences visibly differ.

No simulator code was changed in this phase.

## Phase 7
AI was used to run and document the evaluator audit: WSL build, Norminette, forbidden-function scan, Valgrind Memcheck, Helgrind, burnout smoke, no-relink check, and manual lock-order review.

No simulator code was changed in this phase.

## Phase 8
AI was used to replace the README placeholder sections with concise defense-ready explanations for usage, scheduler policy, synchronization, deadlock prevention, starvation strategy, monitor stop handling, blocking cases, validation evidence, and AI transparency.

No simulator code was changed in this phase.

## Phase 9
AI was used for a Ponytail simplification pass that removed unused per-dongle mutex state, initialization, destruction, prototypes, and documentation references. Central `state_lock` arbitration remains unchanged.
