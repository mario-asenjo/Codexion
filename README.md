*This project has been created as part of the 42 curriculum by masenjo.*

# Codexion

## Description

Codexion is a C89 concurrency simulation built with POSIX threads. It models a circular group of coders sharing USB dongles: every coder needs the two adjacent dongles at the same time to compile, then releases them, debugs, refactors, and tries to compile again.

The objective is to coordinate concurrent access to shared resources without data races, deadlocks, duplicated dongle ownership, interleaved log lines, or starvation in viable EDF scenarios. The simulation stops when either one coder reaches the burnout deadline or every coder has completed at least `number_of_compiles_required` compilations.

Scheduling is performed with custom binary min-heaps:

- `fifo` orders competing requests by arrival sequence;
- `edf` orders them by the earliest burnout deadline, calculated as `last_compile_start + time_to_burnout`.

Each dongle owns its own waiting heap, so arbitration is local to the resource being contested. Independent coder pairs are not forced to wait behind an unrelated request for different dongles.

The implementation keeps all mutable simulation state inside the `t_sim` object and its owned coder/dongle structures. No mutable global variable manages shared resources, scheduling, logging, or stop state.

## Instructions

### Requirements

A POSIX-compatible environment with `cc`, `make`, and POSIX threads.

### Compilation

```sh
make
```

The Makefile builds:

```sh
./codexion
```

with:

```txt
-Wall -Wextra -Werror -pthread
```

Available rules:

```sh
make
make clean
make fclean
make re
```

### Execution

The program requires exactly eight arguments:

```sh
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

1. `number_of_coders` - number of coder threads and dongles.
2. `time_to_burnout` - maximum time in milliseconds between compilation starts before burnout.
3. `time_to_compile` - compilation duration while holding both adjacent dongles.
4. `time_to_debug` - debugging duration.
5. `time_to_refactor` - refactoring duration.
6. `number_of_compiles_required` - minimum compilations every coder must reach for successful termination.
7. `dongle_cooldown` - period after release during which a dongle cannot be acquired.
8. `scheduler` - exactly `fifo` or `edf`.

Example:

```sh
./codexion 5 1000 80 120 220 3 40 edf
```

Invalid argument counts, negative values, malformed numeric inputs, empty numeric inputs, out-of-range values, and schedulers other than `fifo` or `edf` are rejected.

### Log format

State changes use only:

```txt
timestamp_in_ms X has taken a dongle
timestamp_in_ms X is compiling
timestamp_in_ms X is debugging
timestamp_in_ms X is refactoring
timestamp_in_ms X burned out
```

Timestamps are relative to simulation start and coder IDs begin at `1`.

## Resources

References used for the project include:

- [POSIX Threads Programming - pthreads(7)](https://man7.org/linux/man-pages/man7/pthreads.7.html)
- [pthread_create(3)](https://man7.org/linux/man-pages/man3/pthread_create.3.html) and [pthread_join(3)](https://man7.org/linux/man-pages/man3/pthread_join.3.html)
- [pthread_mutex_lock(3p)](https://man7.org/linux/man-pages/man3/pthread_mutex_lock.3p.html)
- [gettimeofday(2)](https://man7.org/linux/man-pages/man2/gettimeofday.2.html)
- Coffman, Elphick and Shoshani, *System Deadlocks* (1971)
- Liu and Layland, *Scheduling Algorithms for Multiprogramming in a Hard-Real-Time Environment* (1973)

### Use of AI

AI was used as a mentoring, review, documentation, and validation aid. It helped to distill the subject and evaluation criteria, review concurrency architecture, design/review the manual parser and custom heaps, inspect thread lifecycle and cleanup, prepare scheduler/burnout tests, review Norminette and forbidden-function risks, and harden evaluator-sensitive edge cases.

The implementation remains intended to be compiled, tested, reviewed, and explainable by the student. A phase-by-phase record is available in [`docs/AI_USAGE_LOG.md`](docs/AI_USAGE_LOG.md).

## Blocking cases handled

### Deadlock and Coffman conditions

The classic circular-wait failure occurs when every coder owns one dongle and waits indefinitely for the second. Codexion avoids that pattern for multi-coder runs by granting the complete adjacent pair atomically: a coder receives both required dongles together or receives neither.

Pair operations follow one lock order: shared `state_lock` first, then dongle mutexes in ascending dongle-ID order. No path intentionally acquires the same resources in the reverse order.

### Duplicate dongle ownership and data races

Every dongle has its own `pthread_mutex_t`. Its ownership, cooldown timestamp, and local scheduling heap are accessed while that dongle mutex is held. Pair acquisition additionally occurs while `state_lock` is held, making the two-resource grant atomic with respect to competing coders.

### Fair arbitration without global head-of-line blocking

Each dongle has a custom binary min-heap containing the requests that currently need that resource. A coder can acquire its pair only when its request is the highest-priority waiter for both adjacent dongles and both resources are available.

FIFO prioritizes the oldest request sequence. EDF prioritizes the closest burnout deadline. Because queues are per dongle, a blocked request for one resource pair does not serialize unrelated coders using disjoint dongles.

### Starvation strategy

FIFO prevents newer contenders for the same dongle from indefinitely overtaking an older request. EDF gives priority to the contender whose burnout deadline is nearest. Equal-deadline behavior remains isolated in `cx_edf_tie_break` so the evaluator recode can change only the tie-break policy.

### Dongle cooldown

When a coder releases a dongle, its `available_at_ms` becomes the release time plus `dongle_cooldown`. An unowned dongle is still unavailable until that timestamp has passed.

### Precise burnout detection

A dedicated monitor thread checks every coder against:

```txt
last_compile_start_ms + time_to_burnout
```

All coders remain subject to burnout until the simulation reaches the global successful-completion stop. The monitor checks at approximately 1 ms intervals so the burnout line can remain within the required 10 ms tolerance under normal evaluation conditions.

### Serialized logging and coordinated stop

`log_lock` serializes output so two state lines cannot interleave. Once `stop` is set, normal state logs are suppressed and the burnout message is the only permitted terminal state message.

### Single-coder resource case

With one coder, the program still creates one real coder thread plus the separate monitor thread. There is exactly one dongle. The coder may take that one resource, but cannot manufacture a second distinct dongle and therefore never enters the compiling state. The monitor eventually reports burnout and the threads terminate cleanly.

## Thread synchronization mechanisms

### `pthread_t`

Every coder is represented by its own POSIX thread. A separate monitor thread detects burnout and coordinates termination.

### `pthread_mutex_t state_lock`

`state_lock` protects simulation-wide shared state, including the stop flag, completion counters, coder timing/counters, request sequence generation, and the atomic coordination of pair acquisition/release.

### Per-dongle `pthread_mutex_t`

Each dongle owns a mutex protecting its mutable state:

- `owner_id`;
- `available_at_ms`;
- its waiting priority heap.

When two dongles must be inspected together, they are locked in ascending ID order while `state_lock` is already held. This provides a deterministic lock hierarchy and avoids lock-order inversion.

### `pthread_mutex_t log_lock`

`log_lock` serializes output. The implementation never intentionally acquires `state_lock` or a dongle mutex after acquiring `log_lock`.

### Thread-safe coder/monitor communication

Coders do not communicate directly with one another. The monitor and coder threads exchange state only through mutex-protected simulation data. A coder updates `last_compile_start_ms` while holding `state_lock`; the monitor reads the same field under that mutex, so deadline checks cannot race with updates.

## Scheduler implementation

C89 has no standard priority-queue container, so Codexion implements a binary min-heap. Each dongle owns one heap of requests for that resource.

For FIFO, `cx_heap_request_before` prioritizes the lowest request sequence. For EDF, it prioritizes the lowest deadline and delegates equal-deadline ordering to `cx_edf_tie_break`.

Relevant files:

- `coders/heap.c`
- `coders/heap_pop.c`
- `coders/heap_order.c`
- `coders/dongle.c`
- `coders/coder_wait.c`

## Validation evidence

Development/audit material is kept in:

- [`docs/SCHEDULER_VALIDATION.md`](docs/SCHEDULER_VALIDATION.md)
- [`docs/AUDIT_REPORT.md`](docs/AUDIT_REPORT.md)
- [`docs/RECODE_PLAYBOOK.md`](docs/RECODE_PLAYBOOK.md)
- [`docs/AI_USAGE_LOG.md`](docs/AI_USAGE_LOG.md)

Historical audit documents describe the implementation at the phase when they were written; evaluator-hardening changes must be revalidated before final delivery.
