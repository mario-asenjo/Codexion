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

## Repository skill cleanup
AI was used to:
- replace repo-local skill markdown files with AGENTS.md instructions that point agents to public Hermes skills;
- remove the repo-created Ponytail optimizer file in favor of the public `ponytail` skill;
- apply a Ponytail review to delete unused future simulation structs from the current header;
- clarify that `docs/ARCHITECTURE.md` structure examples are design notes, not pre-added scaffolding.

No new simulator behavior was introduced by this cleanup.
