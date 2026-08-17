# EDF scalability and evaluator hardening

## Context

A fresh evaluator-style run exposed a reproducible starvation pattern that was not visible in the small 5-coder smoke tests.

The important observations were:

- `5 3000 ... fifo` and `5 3000 ... edf` behaved correctly;
- a single `100 5000 60 60 60 1 0 edf` run could pass;
- repeated 100-coder runs produced burnouts in many executions;
- `200 5000 60 60 60 1 0 edf` could burn out a coder around the initial 5000 ms deadline;
- Valgrind Memcheck was clean;
- Valgrind Helgrind reported no errors;
- Norminette was clean for the mandatory `coders/` sources.

The repeated failures were therefore not explained by memory corruption, a detected data race, or formatting problems.

## Root cause

The previous implementation stored one priority heap per dongle and required a coder request to be the top request of **both** adjacent dongle heaps at the same time before the pair could be granted.

That rule is stronger than the scheduling requirement and creates a distributed head-of-line blocking problem.

At simulation start every coder has:

```text
last_compile_start_ms = 0
```

so EDF gives every initial request the same deadline:

```text
deadline = 0 + time_to_burnout
```

The normal EDF tie-break prefers lower `coder_id`. With a ring of adjacent resources this creates a priority chain: a coder can win one local comparison and lose the other, so only a small subset of requests can be simultaneously top on both heaps. Progress then advances around the ring instead of using the many disjoint dongle pairs that are actually available.

The effect becomes visible with 100-200 coders: some coders can remain on their initial deadline long enough to burn out even though the resource topology permits much more parallel progress.

## Secondary race removed

The old grant path also had a smaller timing window:

1. both dongles were assigned;
2. `state_lock` was released;
3. the coder thread later entered `cx_compile`;
4. only then was `last_compile_start_ms` updated.

The monitor could theoretically inspect the old deadline between steps 2 and 4.

The new scheduler records `last_compile_start_ms` while the grant is committed under `state_lock`, so resource assignment and the logical start of compilation are one synchronized event from the monitor's perspective.

## New scheduling model

Codexion now keeps one shared heap of **pair requests** in `t_sim`.

A request still has the same scheduler key:

- FIFO: request sequence;
- EDF: `last_compile_start_ms + time_to_burnout`;
- equal EDF deadlines: `cx_edf_tie_break`.

The dispatcher processes requests in heap priority order and greedily grants every currently feasible, non-conflicting pair during the same dispatch pass.

A request that cannot be granted because one of its dongles is currently owned or cooling down is deferred and reinserted with its original priority. This makes the scheduler work-conserving without destroying FIFO/EDF ordering information.

Example with equal deadlines:

```text
request 1 -> pair available -> grant
request 2 -> conflicts with request 1 -> defer
request 3 -> disjoint pair available -> grant
request 4 -> conflicts -> defer
...
```

This turns the initial equal-deadline state into a maximal set of independent pair grants instead of a one-coder-at-a-time wave.

## Dongle protection remains literal

The scheduler heap is global, but dongle state is still local and protected exactly as required:

```text
owner_id
available_at_ms
```

Each `t_dongle` owns its own `pthread_mutex_t`.

Pair state is inspected or updated while:

1. `state_lock` is held;
2. the two dongle mutexes are acquired in ascending dongle ID order.

That preserves the existing deterministic lock hierarchy and avoids lock-order inversion.

## Cooldown

Cooldown semantics are unchanged.

When a coder releases its pair:

```text
available_at_ms = release_time + dongle_cooldown
```

The dispatcher considers a pair feasible only when both dongles are unowned and both cooldown timestamps have elapsed.

## Recode compatibility

The evaluator recode remains isolated in:

```text
coders/heap_order.c
cx_edf_tie_break
```

The submitted implementation still prefers lower `coder_id` for equal EDF deadlines. The recode can change that comparison to prefer the higher ID without changing the dispatcher, heap implementation, cooldown logic, or locking.

## Tester redesign

`tests/tester.c` replaces the earlier ad-hoc black-box tester.

Important corrections:

- one unexpected burnout is reported once; coders that remain below their target because the simulation stopped are printed as diagnostic fallout, not dozens of independent failures;
- EDF is **not** judged by the invented property `EDF burnouts <= FIFO burnouts`;
- FIFO and EDF are validated independently against valid scenarios;
- repeated 100- and 200-coder EDF tests are included specifically to catch the starvation pattern that motivated this fix;
- log grammar, non-decreasing timestamps, two acquisitions before each compile, completion counts, forced burnout, N=1, cooldown and phase timing are checked;
- the recode probe is explicitly informational because OS thread start order prevents a black-box test from proving a real equal-deadline tie in every run;
- `--tools` optionally runs Norminette, Valgrind Memcheck and Valgrind Helgrind when installed.

The tester is intentionally outside the mandatory root Makefile and is built independently from `tests/Makefile`.

## Validation commands

After checking out this branch on the target Linux/42 environment:

```sh
make fclean && make
make

cd tests
make re
cd ..

./tests/tester
./tests/tester ./codexion --tools
```

Targeted regression commands:

```sh
./codexion 100 5000 60 60 60 1 0 edf
./codexion 200 5000 60 60 60 1 0 edf

for i in $(seq 1 20); do
    ./codexion 100 5000 60 60 60 1 0 edf |
        grep "burned out" && echo "FAIL RUN $i"
done
```

Memory/race checks:

```sh
valgrind --leak-check=full --show-leak-kinds=all \
    ./codexion 5 3000 100 100 100 2 50 edf

valgrind --tool=helgrind \
    ./codexion 4 3000 100 100 100 2 50 edf
```

Norm:

```sh
norminette coders/*.c coders/*.h
```

## What is and is not claimed in this PR

The branch was created from the current `main` and the scheduler/tester changes were reviewed structurally. The connector environment used to prepare the PR cannot execute a fresh clone of GitHub inside its sandbox, so target-machine runtime results must be produced with the commands above before merge.

The regression is considered fixed only when the repeated 100/200-coder EDF cases are green on the target environment and the existing Memcheck, Helgrind and Norm checks remain green.
