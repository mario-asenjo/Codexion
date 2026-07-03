# Codexion Project Memory

This file records durable project decisions. It is not a todo list and not a session transcript.

## Stable requirements
- Final binary: `codexion`.
- Sources and headers: `coders/`.
- Build: root `Makefile`, target `make`, flags `-Wall -Wextra -Werror -pthread`.
- Strict C89-compatible style; no C99 conveniences.
- No libft, no bonus unless explicitly requested.
- No mutable globals for config/resources/scheduler/logging/monitor.
- Scheduler must support `fifo` and `edf` using a custom binary heap.
- Cooldown after dongle release is mandatory.
- Burnout log must be emitted no later than 10 ms after real burnout.

## Architectural decisions
- Candidate architecture uses a central `state_lock` to arbitrate acquisition of two dongles atomically.
- Heap comparator owns scheduler policy; EDF tie-breaker must be isolated for recode.
- Logs are serialized by `log_lock` and must be suppressed after stop except for the coordinated burnout line.
- `pthread_cond_broadcast` is used instead of `pthread_cond_signal` because signal is forbidden.
- Phase 1 skeleton keeps final simulation unimplemented intentionally: it only validates arguments, provides base types, time helpers, and a serialized logging helper.
- Numeric parsing is manual because `strtol` and `isdigit` are forbidden; only unsigned decimal digits are accepted, so `+1`, `-1`, `1x`, and empty strings are invalid.
- Current parser range policy: coders `1..200`, burnout `1..1000000` ms, compile/debug/refactor/cooldown `0..1000000` ms, required compiles `1..1000000`.
- Repository workflow now uses feature branches and PRs; `.github/pull_request_template.md` defines the review format.
- Local Windows environment has no `cc` on PATH. `Makefile` still uses required `cc`; Phase 1 was verified with `make re CC=/c/Qt/Tools/llvm-mingw1706_64/bin/clang`.
- `.gitattributes` enforces LF line endings for C, headers, Makefile, and Markdown to avoid Windows CRLF surprises in 42/Linux evaluation.
- All canonical build/test verification should run in WSL Ubuntu 24.04, where `/usr/bin/cc` and `/usr/bin/make` are available.
- Phase 2 heap is a fixed-capacity binary min-heap backed by `malloc`; no `realloc`, `qsort`, or library priority queue is used.
- FIFO ordering is isolated in sequence comparison. EDF ordering compares earliest `deadline_ms` first, then delegates equal-deadline behavior to `cx_edf_tie_break`.
- Current EDF tie-break policy is lower `coder_id` first, then lower request `seq`; recode to prefer higher `coder_id` only needs changing `cx_edf_tie_break`.

## Future decisions to record
- Exact sleep/timed-wait strategy.
- Whether individual `dongle.lock` remains necessary after central arbitration.
- Final file/module breakdown.
