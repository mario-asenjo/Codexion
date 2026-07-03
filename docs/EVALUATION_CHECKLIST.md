# Evaluation Checklist

## Build
- [x] `make` builds `codexion` under WSL/Linux.
- [x] `make re` cleans and rebuilds with available local compiler override.
- [x] No warnings with `-Wall -Wextra -Werror -pthread` under clang verification.
- [x] `Makefile` has `NAME`, `all`, `clean`, `fclean`, `re`.
- [x] No unnecessary relink under WSL/Linux `make`.

## Language and Norm
- [x] C89-compatible declarations and comments in current code.
- [x] No `//` comments in C files.
- [x] No declarations inside `for` statements.
- [x] No VLA.
- [x] No `<stdbool.h>`.
- [ ] 42 Norm style reviewed.

## Functions
- [x] Only authorized functions used in current Phase 2 binary.
- [x] Forbidden function scan passed for current code.
- [x] No libft.

## Parser
- [x] `argc == 9` enforced.
- [x] Negative numbers rejected.
- [x] Non-integers rejected.
- [x] Ambiguous numeric strings rejected.
- [x] Scheduler only `fifo` or `edf`.
- [x] Reasonable ranges enforced for Phase 1 policy.

## Concurrency
- [ ] One thread per coder.
- [ ] Separate monitor thread.
- [ ] No mutable globals for shared state.
- [ ] Shared state protected.
- [ ] Logs protected by log mutex.
- [ ] Lock order documented and followed.
- [ ] No deadlock in manual review.
- [ ] Helgrind/DRD checked if available.

## Scheduler and heap
- [x] Custom binary heap implemented.
- [x] FIFO ordering demonstrated by temporary WSL heap test.
- [x] EDF ordering demonstrated by temporary WSL heap test.
- [x] EDF tie-breaker isolated for recode in `cx_edf_tie_break`.
- [x] No `qsort`.

## Simulation correctness
- [ ] One coder case handled.
- [ ] Dongle duplication impossible.
- [ ] Cooldown enforced.
- [ ] Stop by burnout handled.
- [ ] Stop by required compiles handled.
- [ ] No normal logs after stop.
- [ ] Starvation avoided for viable parameters as far as design allows.

## Timing
- [ ] Timestamps are relative to simulation start.
- [ ] Burnout logged within 10 ms reasonably under evaluation timings.
- [ ] No use of forbidden timing functions.
- [ ] Busy-wait minimized.

## Memory
- [ ] All mallocs freed.
- [ ] Mutexes destroyed.
- [ ] Condition variables destroyed.
- [ ] Valgrind leak check clean.

## Defense
- [ ] README complete.
- [x] AI usage log honest.
- [x] Public skill usage is registered in `AGENTS.md`; no repo-local skill files remain.
- [ ] User can explain deadlock prevention.
- [ ] User can explain starvation strategy.
- [ ] User can explain heap comparator.
- [ ] User can explain monitor deadline detection.
- [ ] Recode EDF tie-breaker rehearsed.
