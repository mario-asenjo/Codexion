# Specification Distilled

## Program
- Name: `codexion`.
- Build: `make`.
- Flags: `-Wall -Wextra -Werror -pthread`.
- Language: C, C89-compatible style.
- Source/header location: `coders/`.
- No libft.

## Arguments
Exactly 8 user arguments, so `argc == 9`:

```txt
number_of_coders time_to_burnout time_to_compile time_to_debug time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

Reject incorrect count, negative numbers, non-integers, ambiguous parse, unreasonable ranges, and scheduler other than `fifo` or `edf`.

## Simulation
- `number_of_coders` coders and dongles.
- One coder thread per coder.
- One separate monitor thread.
- Dongles form a circle, one between each pair of coders.
- One coder case has only one dongle, so compiling is impossible and burnout must be handled correctly.
- Compiling requires two dongles simultaneously.
- After release, each dongle is unavailable for `dongle_cooldown` ms.
- Scheduler must be fair and implemented with a custom heap.
- FIFO priority: request arrival sequence.
- EDF priority: closest deadline, where `deadline = last_compile_start + time_to_burnout`.

## Stop conditions
- Any coder burns out.
- Or every coder reaches `number_of_compiles_required` compilations.

## Logs
Exact normal logs:
- `timestamp_in_ms X has taken a dongle`
- `timestamp_in_ms X is compiling`
- `timestamp_in_ms X is debugging`
- `timestamp_in_ms X is refactoring`
- `timestamp_in_ms X burned out`

Burnout must be logged within 10 ms of real burnout. Logs must be serialized.

## Zero-grade risks
- Build error or warning.
- Segfault, bus error, double free, unexpected termination.
- Data race.
- Deadlock.
- Mixed log lines.
- Memory leak.
- Forbidden function.
- Mutable global resource/scheduler/logging/monitor state.
