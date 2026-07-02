# Codexion Roadmap

## Phase 0 — Agent bootstrap
- Create AGENTS/MEMORY/SOUL/skills/docs.
- Create README template.
- Create checklist of evaluation risks.
- Do not implement the final solution yet.

Done when the repository contains the project brain and `coders/.gitkeep` only under source code.

## Phase 1 — Project skeleton
- Makefile.
- Headers.
- Base types.
- Strict parser.
- Time utilities.
- Logging utility.
- Clean compilation.

Done when `make re` builds `codexion` without warnings and a minimal argument validation path runs.

Status: implemented on branch `feat/phase-1-project-skeleton`. Verified with local clang override because this Windows PATH currently has no `cc` binary available.

## Phase 2 — Heap scheduler
- Custom binary heap.
- FIFO comparator.
- EDF comparator.
- Isolated tie-breaker.
- Small manual heap tests, not part of final delivery or clearly separated.

Done when heap ordering is demonstrable for FIFO and EDF.

## Phase 3 — Simulation state
- Initialize coders and dongles.
- Zero mutable globals.
- Memory ownership documented.
- Destroy all mutexes/conds/resources.

Done when init/destroy survives leak checks on simple paths.

## Phase 4 — Coder lifecycle
- Compile/debug/refactor loop.
- Atomic acquisition/release of two dongles.
- Cooldown.
- Exact logs.

Done when small viable cases complete without mixed logs or obvious starvation.

## Phase 5 — Monitor
- Deadline detection.
- Stop flag.
- Broadcast on stop.
- Burnout precision <= 10 ms under evaluation timings.

Done when forced burnout cases print one coordinated burnout line quickly.

## Phase 6 — EDF/FIFO validation
- Viable cases.
- Burnout cases.
- Scheduler differences.
- No dongle duplication.
- No mixed logs.

Done when manual scenarios demonstrate both policies.

## Phase 7 — Memory/race/deadlock audit
- Valgrind leaks.
- Helgrind/DRD if available.
- Manual lock-order review.
- Forbidden function scan.

Done when no zero-grade issue remains known.

## Phase 8 — README and defense
- Final README.
- Honest AI usage log.
- Oral explanation.
- Blocking cases handled.
- Thread synchronization mechanisms.

Done when the project can be defended without reading code line by line.

## Phase 9 — Ponytail optimization
- Only after green tests.
- Reduce duplication.
- Improve names.
- Lower accidental complexity.
- No behavior change without tests.

Done when code is simpler and still passes the same checks.

## Phase 10 — Recode readiness
- Practice EDF tie-breaker change.
- Verify it can be changed in minutes.
- Document exact function to touch and why.

Done when recode playbook has been rehearsed.
