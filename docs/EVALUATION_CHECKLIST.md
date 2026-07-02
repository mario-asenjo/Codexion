# Evaluation Checklist

## Build
- [ ] `make` builds `codexion`.
- [x] `make re` cleans and rebuilds with available local compiler override.
- [x] No warnings with `-Wall -Wextra -Werror -pthread` under clang verification.
- [x] `Makefile` has `NAME`, `all`, `clean`, `fclean`, `re`.
- [ ] No unnecessary relink.

## Language and Norm
- [ ] C89-compatible declarations and comments.
- [ ] No `//` comments in C files.
- [ ] No declarations inside `for` statements.
- [ ] No VLA.
- [ ] No `<stdbool.h>`.
- [ ] 42 Norm style reviewed.

## Functions
- [ ] Only authorized functions used in final binary.
- [ ] Forbidden function scan passed.
- [ ] No libft.

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
- [ ] Custom binary heap implemented.
- [ ] FIFO ordering demonstrated.
- [ ] EDF ordering demonstrated.
- [ ] EDF tie-breaker isolated for recode.
- [ ] No `qsort`.

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
- [ ] AI usage log honest.
- [ ] User can explain deadlock prevention.
- [ ] User can explain starvation strategy.
- [ ] User can explain heap comparator.
- [ ] User can explain monitor deadline detection.
- [ ] Recode EDF tie-breaker rehearsed.
