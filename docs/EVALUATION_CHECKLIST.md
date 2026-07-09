# Evaluation Checklist

## Build
- [x] `make` builds `codexion` under WSL/Linux.
- [x] `make re` cleans and rebuilds with available local compiler override.
- [x] No warnings with `-Wall -Wextra -Werror -pthread` under WSL verification.
- [x] `Makefile` has `NAME`, `all`, `clean`, `fclean`, `re`.
- [x] No unnecessary relink under WSL/Linux `make`.

## Language and Norm
- [x] C89-compatible declarations and comments.
- [x] No `//` comments in C files.
- [x] No declarations inside `for` statements.
- [x] No VLA.
- [x] No `<stdbool.h>`.
- [x] `norminette coders` passes in WSL.

## Functions
- [x] Only authorized functions used in current binary.
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
- [x] One thread per coder in viable Phase 4 cases.
- [x] Separate monitor thread.
- [x] No mutable globals for shared state.
- [x] Shared simulation state ownership initialized without globals.
- [x] Logs protected by log mutex.
- [x] Lock order documented and followed for current Phase 5 paths.
- [ ] No deadlock in manual review.
- [ ] Helgrind/DRD checked if available.

## Scheduler and heap
- [x] Custom binary heap implemented.
- [x] FIFO ordering demonstrated by temporary WSL heap test.
- [x] EDF ordering demonstrated by temporary WSL heap test.
- [x] FIFO/EDF runtime behavior differs on documented Phase 6 scenario.
- [x] EDF tie-breaker isolated for recode in `cx_edf_tie_break`.
- [x] No `qsort`.

## Simulation correctness
- [x] One coder case handled by monitor burnout.
- [x] Dongle duplication prevented by atomic ownership under `state_lock`.
- [x] Phase 4 cooldown enforced by `available_at_ms` before grant.
- [x] Stop by burnout handled.
- [x] Stop by required compiles handled for viable Phase 4 cases.
- [x] No normal logs after stop.
- [x] Starvation avoided for documented viable Phase 6 scenarios.

## Timing
- [x] Timestamps are relative to simulation start.
- [x] Burnout logged within 10 ms reasonably under evaluation timings.
- [x] No use of forbidden timing functions.
- [x] Busy-wait minimized with bounded 1 ms sleeps in current timing paths.

## Memory
- [x] Phase 3 malloc ownership freed by `cx_sim_destroy`.
- [x] Phase 3 mutexes destroyed by `cx_sim_destroy`.
- [x] Phase 3 condition variable destroyed by `cx_sim_destroy`.
- [ ] Valgrind leak check clean. Blocked locally: Valgrind missing in WSL; AddressSanitizer fallback passed for init/destroy.

## Defense
- [ ] README complete.
- [x] AI usage log honest.
- [x] Public skill usage is registered in `AGENTS.md`; no repo-local skill files remain.
- [ ] User can explain deadlock prevention.
- [ ] User can explain starvation strategy.
- [ ] User can explain heap comparator.
- [x] User can explain monitor deadline detection.
- [ ] Recode EDF tie-breaker rehearsed.
