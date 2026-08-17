*This project has been created as part of the 42 curriculum by masenjo.*

# Codexion

## Description

Codexion is a C89 concurrency simulation built with POSIX threads. It models a circular group of coders sharing USB dongles: every coder needs the two adjacent dongles at the same time to compile, then releases them, debugs, refactors, and tries to compile again.

The objective is to coordinate concurrent access to shared resources without data races, deadlocks, duplicated dongle ownership, interleaved log lines, or starvation in viable EDF scenarios. The simulation stops when either one coder reaches the burnout deadline or every coder has completed at least `number_of_compiles_required` compilations.

Scheduling is performed with a custom binary min-heap of pending **pair requests**:

- `fifo` orders requests by arrival sequence;
- `edf` orders them by the earliest burnout deadline, calculated as `last_compile_start + time_to_burnout`.

The dispatcher processes requests in scheduler order and grants the earliest currently feasible, non-conflicting pairs. Requests blocked because one adjacent dongle is owned or cooling down keep their original priority and are reconsidered on the next dispatch pass. This avoids the distributed head-of-line blocking that appears when a coder must be the top request of two independent dongle queues simultaneously.

Each dongle still owns its own `pthread_mutex_t`, protecting its owner and cooldown state. Pair updates happen under the shared simulation lock and the two dongle mutexes are acquired in ascending dongle-ID order.

The implementation keeps all mutable simulation state inside the `t_sim` object and its owned coder/dongle structures. No mutable global variable manages shared resources, scheduling, logging, or stop state.

## Instructions

### Requirements

A POSIX-compatible environment with `cc`, `make`, and POSIX threads.

### Compilation

```sh
make
```

The root Makefile builds:

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

### Standalone evaluator tester

A black-box C tester is kept outside the mandatory root build:

```sh
make -C tests
./tests/tester
```

Optional external-tool checks:

```sh
./tests/tester ./codexion --tools
```

The tester covers the Easy/Less Easy/Medium evaluation areas, repeated 100/200-coder EDF stress, logging, completion counts, cooldown, burnout timing and the N=1 case. It deliberately avoids multiplying one global-stop failure into dozens of derived failures and does not invent a requirement that EDF must statistically produce fewer burnouts than FIFO.

See [`tests/README.md`](tests/README.md) for details.

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

The classic circular-wait failure occurs when every coder owns one dongle and waits indefinitely for the second. Codexion avoids that pattern by granting the complete adjacent pair atomically: a coder receives both required dongles together or receives neither.

Pair state changes follow one lock order: shared `state_lock` first, then dongle mutexes in ascending dongle-ID order. No path intentionally acquires the same resources in the reverse order.

### Duplicate dongle ownership and data races

Every dongle has its own `pthread_mutex_t`. Its ownership and cooldown timestamp are accessed while that dongle mutex is held. Pair acquisition additionally occurs while `state_lock` is held, making the two-resource grant atomic with respect to competing coders and the monitor-visible compile deadline.

### Fair arbitration without distributed head-of-line blocking

Pending coder requests live in one custom binary min-heap. The heap defines the scheduler order, while the dispatcher greedily grants every currently feasible non-conflicting pair in that order.

FIFO prioritizes the oldest request sequence. EDF prioritizes the closest burnout deadline. A request that is temporarily infeasible because one of its resources is occupied or cooling down is deferred without losing its scheduler key.

This distinction is important: the earlier two-independent-heaps design required one request to be the local winner of two dongle queues at the same time. With equal initial EDF deadlines and deterministic tie-breaking that could serialize progress around the ring even though many disjoint pairs were free. The feasible-pair dispatcher removes that artificial blocking.

### Starvation strategy

FIFO preserves request sequence for pending requests. EDF preserves each request's burnout deadline and always considers earlier deadlines before later ones. Feasibility skipping prevents an unrelated blocked pair from stopping disjoint work, while deferred requests retain their original priority for the next dispatch pass.

Equal-deadline behavior remains isolated in `cx_edf_tie_break` so the evaluator recode can change only the tie-break policy.

### Dongle cooldown

When a coder releases a dongle, its `available_at_ms` becomes the release time plus `dongle_cooldown`. An unowned dongle is still unavailable until that timestamp has passed. The dispatcher grants a pair only when both adjacent dongles are unowned and both cooldown timestamps have elapsed.

### Precise burnout detection

A dedicated monitor thread checks every coder against:

```txt
last_compile_start_ms + time_to_burnout
```

The logical compile start is now committed at the same synchronized moment as the pair grant. This removes the previous window where both dongles could already be assigned while the monitor still observed the old `last_compile_start_ms`.

All coders remain subject to burnout until the simulation reaches the global successful-completion stop. The monitor checks at approximately 1 ms intervals so the burnout line can remain within the required 10 ms tolerance under normal evaluation conditions.

### Serialized logging and coordinated stop

`log_lock` serializes output so two state lines cannot interleave. Once `stop` is set, normal state logs are suppressed and the burnout message is the only permitted terminal state message.

### Single-coder resource case

With one coder, the program still creates one real coder thread plus the separate monitor thread. There is exactly one dongle. The coder may take that one resource, but cannot manufacture a second distinct dongle and therefore never enters the compiling state. The monitor eventually reports burnout and the threads terminate cleanly.

## Thread synchronization mechanisms

### `pthread_t`

Every coder is represented by its own POSIX thread. A separate monitor thread detects burnout and coordinates termination.

### `pthread_mutex_t state_lock`

`state_lock` protects simulation-wide shared state, including the stop flag, completion counters, coder timing/counters, request sequence generation, the shared scheduling heap and atomic coordination of pair acquisition/release.

### Per-dongle `pthread_mutex_t`

Each dongle owns a mutex protecting its mutable state:

- `owner_id`;
- `available_at_ms`.

When two dongles must be inspected together, they are locked in ascending ID order while `state_lock` is already held. This provides a deterministic lock hierarchy and avoids lock-order inversion.

### `pthread_mutex_t log_lock`

`log_lock` serializes output. The implementation never intentionally acquires a dongle mutex after acquiring `log_lock`.

### Thread-safe coder/monitor communication

Coders do not communicate directly with one another. The monitor and coder threads exchange state only through mutex-protected simulation data. The scheduler writes `last_compile_start_ms` while holding `state_lock`; the monitor reads the same field under that mutex, so deadline checks cannot race with the logical start of a compilation.

## Scheduler implementation

C89 has no standard priority-queue container, so Codexion implements a binary min-heap.

For FIFO, `cx_heap_request_before` prioritizes the lowest request sequence. For EDF, it prioritizes the lowest deadline and delegates equal-deadline ordering to `cx_edf_tie_break`.

`cx_dispatch_grants` consumes pending requests in heap order. Feasible pairs are reserved immediately; blocked requests are retained and reinserted with their original sequence/deadline. This allows disjoint coders to progress during the same dispatch pass without changing the scheduler key.

Relevant files:

- `coders/heap.c`
- `coders/heap_pop.c`
- `coders/heap_order.c`
- `coders/coder_wait.c`
- `coders/coder_grant.c`
- `coders/dongle.c`

## Validation evidence

Development/audit material is kept in:

- [`docs/SCHEDULER_VALIDATION.md`](docs/SCHEDULER_VALIDATION.md)
- [`docs/AUDIT_REPORT.md`](docs/AUDIT_REPORT.md)
- [`docs/EDF_SCALABILITY_FIX.md`](docs/EDF_SCALABILITY_FIX.md)
- [`docs/RECODE_PLAYBOOK.md`](docs/RECODE_PLAYBOOK.md)
- [`docs/AI_USAGE_LOG.md`](docs/AI_USAGE_LOG.md)
- [`tests/README.md`](tests/README.md)

The pre-fix runtime evidence showed clean Memcheck, Helgrind and Norminette results but reproducible EDF starvation under repeated 100/200-coder stress. The scalability fix must therefore be revalidated on the target Linux/42 environment before merge. The exact commands and expected checks are documented in `docs/EDF_SCALABILITY_FIX.md`.
