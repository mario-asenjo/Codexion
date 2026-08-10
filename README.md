*This project has been created as part of the 42 curriculum by masenjo.*

# Codexion

## Description

Codexion is a C89 concurrency simulation built with POSIX threads. It models a circular group of coders sharing a limited set of USB dongles: every coder needs the two adjacent dongles at the same time to compile, then releases them, debugs, refactors, and tries to compile again.

The objective is to coordinate concurrent access to shared resources without data races, deadlocks, duplicated dongle ownership, interleaved log lines, or starvation in viable EDF scenarios. The simulation stops when either:

- one coder reaches the burnout deadline without starting the next compilation; or
- every coder has completed at least `number_of_compiles_required` compilations.

The program implements two scheduling policies through a custom binary min-heap:

- `fifo` - requests are ordered by arrival sequence;
- `edf` - requests are ordered by the earliest burnout deadline, calculated as `last_compile_start + time_to_burnout`.

The implementation keeps all mutable simulation state inside a central `t_sim` object; no mutable global variable is used to manage dongles, scheduling, logging, or stop state.

## Instructions

### Requirements

A POSIX-compatible environment with:

- `cc`;
- `make`;
- POSIX threads.

### Compilation

```sh
make
```

The Makefile builds the executable:

```sh
./codexion
```

The project is compiled with:

```txt
-Wall -Wextra -Werror -pthread
```

Available Makefile rules:

```sh
make        # build codexion
make clean  # remove object files
make fclean # remove object files and executable
make re     # rebuild from scratch
```

### Execution

The program requires exactly eight arguments, in this order:

```sh
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

Arguments:

1. `number_of_coders` - number of coder threads and number of dongles.
2. `time_to_burnout` - maximum time in milliseconds between compilation starts before a coder burns out.
3. `time_to_compile` - compilation duration in milliseconds; both adjacent dongles are held during this phase.
4. `time_to_debug` - debugging duration in milliseconds.
5. `time_to_refactor` - refactoring duration in milliseconds.
6. `number_of_compiles_required` - number of successful compilations required from every coder before successful termination.
7. `dongle_cooldown` - time in milliseconds for which a released dongle remains unavailable.
8. `scheduler` - exactly `fifo` or `edf`.

Example:

```sh
./codexion 5 1000 80 120 220 3 40 edf
```

Invalid argument counts, negative values, non-integer numeric inputs, empty numeric inputs, out-of-range values, and schedulers other than `fifo` or `edf` are rejected.

### Log format

State changes use only the required formats:

```txt
timestamp_in_ms X has taken a dongle
timestamp_in_ms X is compiling
timestamp_in_ms X is debugging
timestamp_in_ms X is refactoring
timestamp_in_ms X burned out
```

Timestamps are relative to the simulation start and coder IDs begin at `1`.

## Resources

References used to understand the concurrency concepts and POSIX primitives behind the project:

- [POSIX Threads Programming - pthreads(7)](https://man7.org/linux/man-pages/man7/pthreads.7.html) - overview of the POSIX threads model.
- [pthread_create(3)](https://man7.org/linux/man-pages/man3/pthread_create.3.html) and [pthread_join(3)](https://man7.org/linux/man-pages/man3/pthread_join.3.html) - thread creation and lifecycle management.
- [pthread_mutex_lock(3p)](https://man7.org/linux/man-pages/man3/pthread_mutex_lock.3p.html) - mutual exclusion and protection of shared state.
- [pthread_cond_wait(3p)](https://man7.org/linux/man-pages/man3/pthread_cond_wait.3p.html) and [pthread_cond_broadcast(3p)](https://man7.org/linux/man-pages/man3/pthread_cond_broadcast.3p.html) - condition-variable synchronization primitives.
- [gettimeofday(2)](https://man7.org/linux/man-pages/man2/gettimeofday.2.html) - wall-clock timing used by the simulation.
- Coffman, Elphick and Shoshani, *System Deadlocks* (1971) - the classical Coffman conditions used to reason about deadlock prevention.
- Liu and Layland, *Scheduling Algorithms for Multiprogramming in a Hard-Real-Time Environment* (1973) - classical real-time scheduling background relevant to Earliest Deadline First.

### Use of AI

AI was used as a mentoring, review, documentation, and validation aid. In particular, it helped to:

- distill the subject and evaluation criteria into implementation checklists;
- review architecture and concurrency decisions;
- design and review the manual argument parser and custom FIFO/EDF heap;
- review thread lifecycle, monitor logic, cooldown handling, lock ordering, and cleanup paths;
- prepare temporary test scenarios for scheduler behavior, burnout, memory leaks, and data races;
- review Norminette compliance and forbidden-function usage;
- improve documentation and defense-oriented explanations.

The implementation was still compiled, tested, reviewed, and kept explainable by the student. A phase-by-phase record is available in [`docs/AI_USAGE_LOG.md`](docs/AI_USAGE_LOG.md).

## Blocking cases handled

### Deadlock and Coffman conditions

The classic circular-wait failure would occur if every coder acquired one dongle and then waited indefinitely for the second one.

Codexion avoids this by making the pair acquisition atomic under `state_lock`: a coder receives both adjacent dongles together or receives none. A thread never keeps one dongle while waiting for the other. This removes the hold-and-wait/circular-wait pattern that would otherwise permit a Coffman deadlock.

### Duplicate dongle ownership and data races

Dongle ownership and cooldown timestamps are read and modified only while `state_lock` is held. Two coders therefore cannot successfully claim the same dongle concurrently, and shared dongle state is not accessed through unsynchronized writes.

### Starvation and scheduling fairness

Every compile request is inserted into a custom binary min-heap:

- FIFO prioritizes the oldest request sequence;
- EDF prioritizes the coder with the closest burnout deadline.

EDF therefore gives urgency to coders closest to burnout, while FIFO preserves request order. The scheduler behavior is documented with runtime examples in [`docs/SCHEDULER_VALIDATION.md`](docs/SCHEDULER_VALIDATION.md).

### Dongle cooldown

When a coder releases its two dongles, both receive an `available_at_ms` timestamp equal to the release time plus `dongle_cooldown`. A request can be granted only when both adjacent dongles are unowned and their cooldown deadlines have expired.

### Precise burnout detection

A dedicated monitor thread repeatedly checks coder deadlines using:

```txt
last_compile_start_ms + time_to_burnout
```

When burnout is detected, the monitor sets the shared stop state under `state_lock`, wakes state-change listeners, and emits the single `burned out` log line. The monitor checks at approximately 1 ms intervals so the burnout message can remain within the required 10 ms tolerance under normal evaluation conditions.

### Serialized logging and coordinated stop

`log_lock` guarantees that two threads cannot write a log line at the same time. Normal state messages are suppressed after the shared stop flag is set, so the final burnout event is not followed by unrelated coder-state logs.

### Single-coder resource case

With one coder there is only one dongle, so acquiring the two distinct resources required for compilation is impossible. The simulation therefore ends through burnout rather than duplicating the single dongle.

## Thread synchronization mechanisms

Codexion uses a central shared simulation object and the following POSIX synchronization primitives.

### `pthread_t`

Coder execution is modeled with POSIX threads, and a separate monitor thread is responsible for detecting burnout and coordinating termination.

### `pthread_mutex_t state_lock`

`state_lock` protects the shared simulation state, including:

- heap insertion/removal and request ordering;
- dongle ownership;
- dongle cooldown timestamps;
- each coder's compilation counters and last compilation start;
- the global stop flag;
- completion counters;
- monitor-visible state.

Example: a coder checks that both adjacent dongles are free and out of cooldown, removes its request from the heap, and assigns both dongle owners while holding the same mutex. No competing thread can observe and modify that grant halfway through, which prevents a race that could duplicate ownership.

### `pthread_mutex_t log_lock`

`log_lock` serializes output. The implementation follows the lock order:

```txt
state_lock -> log_lock
```

No code path intentionally acquires these locks in the reverse order, avoiding lock-order inversion.

### `pthread_cond_t state_changed`

A shared condition variable represents state-change notifications. The current implementation broadcasts it when important shared state changes, such as dongle release or coordinated stop. Coder acquisition currently rechecks eligibility with a bounded 1 ms polling loop rather than blocking in `pthread_cond_wait`, so the README does not rely on the condition variable as the sole waiting mechanism.

### Thread-safe coder/monitor communication

Coders do not communicate directly with one another. Their coordination happens only through shared state protected by `state_lock`.

The monitor reads coder deadlines and writes the stop state under the same mutex used by coder threads. This means a coder cannot concurrently update `last_compile_start_ms` while the monitor reads a partially synchronized value. After stop is set, coder threads observe the same protected flag and leave their loops cleanly.

## Scheduler implementation

The waiting requests are stored in a custom binary min-heap because C89 provides no standard priority-queue container.

For FIFO, the comparator prioritizes the lowest request sequence number. For EDF, it prioritizes the lowest deadline and delegates equal-deadline ordering to `cx_edf_tie_break`.

Relevant implementation files:

- `coders/heap.c`
- `coders/heap_pop.c`
- `coders/heap_order.c`
- `coders/coder_wait.c`

## Validation evidence

The repository contains additional audit material used during development:

- [`docs/SCHEDULER_VALIDATION.md`](docs/SCHEDULER_VALIDATION.md) - FIFO/EDF runtime scenarios and expected ordering.
- [`docs/AUDIT_REPORT.md`](docs/AUDIT_REPORT.md) - build, Norminette, forbidden-function scan, Valgrind Memcheck, Helgrind, burnout, and no-relink checks.
- [`docs/RECODE_PLAYBOOK.md`](docs/RECODE_PLAYBOOK.md) - preparation for the EDF equal-deadline tie-breaker recode.
- [`docs/AI_USAGE_LOG.md`](docs/AI_USAGE_LOG.md) - detailed record of AI assistance.
