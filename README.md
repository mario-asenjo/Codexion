*This project has been created as part of the 42 curriculum by < masenjo >.*

# Codexion

## Description
Codexion simulates several coders working around a circular hub. Each coder needs two USB dongles at the same time to compile quantum code. While compiling, a coder does not debug or refactor. After compiling, the coder releases both dongles, debugs, refactors, and tries to compile again.

The simulation stops when one coder burns out because they did not start compiling before their deadline, or when every coder has completed the required number of compilations.

This project is a concurrency exercise inspired by the dining philosophers problem, with additional constraints: dongle cooldown, FIFO/EDF scheduling, a custom priority heap, a separate monitor thread, serialized logs, and strict timing.

## Instructions
Build:

```sh
make
```

Clean:

```sh
make clean
make fclean
make re
```

Run:

```sh
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

Example:

```sh
./codexion 5 800 100 100 100 3 60 edf
```

Arguments:
1. `number_of_coders`: number of coders and dongles.
2. `time_to_burnout`: maximum milliseconds from the beginning of the last compilation, or simulation start, until the next compilation starts.
3. `time_to_compile`: milliseconds spent compiling while holding two dongles.
4. `time_to_debug`: milliseconds spent debugging.
5. `time_to_refactor`: milliseconds spent refactoring.
6. `number_of_compiles_required`: simulation succeeds when all coders reach at least this count.
7. `dongle_cooldown`: milliseconds during which a released dongle remains unavailable.
8. `scheduler`: `fifo` or `edf`.

## Resources
- POSIX Threads: `pthread_create`, `pthread_join`, mutexes, condition variables.
- `gettimeofday` for timestamps.
- Dining philosophers deadlock/starvation analysis.
- Binary heaps and priority queues.
- 42 Norm and C89-compatible C style.

## Blocking cases handled
This section must be completed during implementation. It should explain:
- one coder with one dongle;
- invalid argument count;
- negative and non-integer values;
- invalid scheduler;
- impossible or burnout-prone timings;
- stop caused by burnout;
- stop caused by all coders reaching the required compilation count;
- dongle cooldown delaying future acquisition;
- no logs after coordinated simulation stop.

## Thread synchronization mechanisms
This section must be completed with implementation details. The intended design is:
- one thread per coder;
- one separate monitor thread;
- a central simulation state mutex for scheduler, stop flag, counters, and dongle availability arbitration;
- a condition variable to wake waiting coders and monitor when state changes;
- `pthread_cond_broadcast`, not `pthread_cond_signal`, because signal is not authorized;
- a logging mutex to serialize every log line;
- a documented lock order to avoid deadlocks.

## Deadlock prevention
The recommended design avoids the classic circular wait by not allowing coders to independently hold one dongle while waiting for the next. A central arbiter grants both required dongles atomically only when the scheduler and cooldown rules allow it.

In Coffman terms, mutual exclusion still exists, but circular wait is removed by central arbitration and a documented lock order.

## Starvation prevention
FIFO grants priority by request sequence. EDF grants priority by closest compilation deadline. The scheduler is implemented with a custom binary heap so selection is explicit and inspectable.

## Cooldown
When a coder releases a dongle, the dongle receives an `available_at_ms` timestamp. It cannot be granted again before this timestamp. The simulation must re-check cooldown expirations with accurate waits, not aggressive busy-wait.

## Logs
The required log format is exactly:

```txt
timestamp_in_ms X has taken a dongle
timestamp_in_ms X is compiling
timestamp_in_ms X is debugging
timestamp_in_ms X is refactoring
timestamp_in_ms X burned out
```

`X` starts at 1. Timestamps are milliseconds since simulation start. Log lines must never mix.

## AI usage
AI may be used to structure plans, review concurrency risks, generate checklists, and explain concepts. The student remains responsible for understanding, validating, and defending every line of code. See `docs/AI_USAGE_LOG.md`.

## Placeholder reminder
Replace `<<LOGIN_42>>` with the actual 42 login before final delivery.
