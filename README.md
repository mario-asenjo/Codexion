*This project has been created as part of the 42 curriculum by < masenjo >.*

# Codexion

Codexion is a C89/POSIX Threads concurrency simulator inspired by dining philosophers.

Coders sit around a circular hub. Each coder needs both adjacent USB dongles to compile. After compiling, the coder releases both dongles, debugs, refactors, and asks to compile again. The simulation stops when one coder burns out or when every coder reaches the required compile count.

## Build

```sh
make
make re
make clean
make fclean
```

The binary is:

```sh
./codexion
```

## Usage

```sh
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

Example:

```sh
./codexion 5 800 100 100 100 3 60 edf
```

Arguments:

1. `number_of_coders`: number of coder threads and dongles.
2. `time_to_burnout`: milliseconds allowed before a coder must start compiling again.
3. `time_to_compile`: milliseconds spent compiling while holding two dongles.
4. `time_to_debug`: milliseconds spent debugging after compiling.
5. `time_to_refactor`: milliseconds spent refactoring after debugging.
6. `number_of_compiles_required`: successful stop target for every coder.
7. `dongle_cooldown`: milliseconds a released dongle stays unavailable.
8. `scheduler`: exactly `fifo` or `edf`.

## Scheduler

The scheduler is a custom binary min-heap of compile requests.

- `fifo`: earlier request sequence wins.
- `edf`: earliest deadline wins, where deadline is `last_compile_start_ms + time_to_burnout`.
- EDF equal-deadline tie-breaks in `cx_edf_tie_break`.

This keeps heap mechanics separate from scheduling policy.

## Synchronization

Codexion uses one central simulation object. There are no mutable global resources.

- One thread per coder.
- One monitor thread.
- `state_lock` protects scheduler heap access, stop state, counters, and dongle ownership/cooldown.
- `log_lock` serializes log lines.
- `pthread_cond_broadcast` wakes waiters because `pthread_cond_signal` is forbidden.

Lock order:

```txt
state_lock -> log_lock
```

Dongle mutexes exist as owned resources but current arbitration uses `state_lock`; no coder locks one dongle while waiting for another.

## Deadlock prevention

The classic deadlock is: every coder holds one dongle and waits forever for the second.

Codexion avoids that by granting two dongles atomically under `state_lock`. A coder either gets both adjacent dongles, or gets none and waits/retries. That removes circular wait.

## Starvation strategy

FIFO prevents request-order starvation by serving older requests first.

EDF prioritizes coders closest to burnout. This can change the compile order after deadlines diverge, which is documented in `docs/SCHEDULER_VALIDATION.md`.

## Monitor and stop

The monitor scans coder deadlines. If a coder reaches burnout:

1. monitor sets `stop` under `state_lock`;
2. monitor broadcasts state changes;
3. one `burned out` line is logged;
4. normal logs are suppressed after stop.

Successful completion is handled when every coder reaches `number_of_compiles_required`.

## Blocking cases handled

- Wrong argument count prints usage and exits non-zero.
- Negative, non-integer, empty, or ambiguous numeric inputs are rejected.
- Scheduler must be exactly `fifo` or `edf`.
- One coder cannot acquire two dongles and burns out correctly.
- Burnout emits exactly one burnout line.
- No normal logs appear after coordinated stop.
- Completion stops when all coders reach the required compile count.
- Dongle cooldown is enforced with `available_at_ms`.
- Dongle ownership is granted atomically, so duplicate adjacent ownership is avoided.

## Logs

Only these log messages are valid:

```txt
timestamp_in_ms X has taken a dongle
timestamp_in_ms X is compiling
timestamp_in_ms X is debugging
timestamp_in_ms X is refactoring
timestamp_in_ms X burned out
```

`X` starts at 1. Timestamps are relative to simulation start.

## Validation evidence

- `docs/SCHEDULER_VALIDATION.md`: FIFO/EDF runtime behavior evidence.
- `docs/AUDIT_REPORT.md`: build, Norminette, forbidden-function scan, Memcheck, Helgrind, and no-relink audit.
- `docs/RECODE_PLAYBOOK.md`: EDF tie-breaker recode rehearsal notes.
- `docs/AI_USAGE_LOG.md`: AI assistance transparency.

## Defense cheat sheet

- Deadlock prevention: coders never hold one dongle while waiting; `state_lock` grants both or none.
- Starvation strategy: FIFO uses request sequence; EDF uses earliest burnout deadline.
- Heap comparator: the heap stores requests, and `cx_heap_request_before` owns FIFO/EDF ordering.
- Monitor: compares `now` against `last_compile_start_ms + time_to_burnout`, then sets stop and logs burnout.
- Logs: `log_lock` serializes output; normal logs are blocked after stop.
