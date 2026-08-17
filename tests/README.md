# Codexion evaluator tester

`tester.c` is a standalone black-box evaluator helper. It is not linked into the mandatory `codexion` binary and the root Makefile does not depend on it.

## Build

From the repository root:

```sh
make
make -C tests
```

## Run

```sh
./tests/tester
```

To test another binary path:

```sh
./tests/tester /path/to/codexion
```

To additionally run Norminette, Valgrind Memcheck and Valgrind Helgrind when those tools are installed:

```sh
./tests/tester ./codexion --tools
```

## What it checks

The harness is organized around the evaluation sections:

- argument validation;
- Easy FIFO/EDF runs;
- repeated 100-coder and 200-coder EDF stress;
- forced burnout and N=1;
- cooldown with N=2;
- scheduler behavior under contention;
- log grammar and timestamp ordering;
- exactly two `has taken a dongle` events before each compile;
- minimum compile/debug/refactor timing;
- successful completion counts;
- burnout terminal-line behavior and deadline tolerance;
- informational EDF recode probe;
- optional external-tool checks.

## Deliberate behavior

The tester does **not** turn one burnout into dozens of failures merely because the global stop prevents the remaining coders from reaching their target. It reports the unexpected burnout and prints the number of incomplete coders as diagnostic fallout.

It also does **not** assert that EDF must statistically produce fewer burnouts than FIFO. That comparison is not an evaluation requirement. Each scheduler is tested independently.

The recode probe is informational rather than a hard assertion because a black-box process cannot guarantee that all equal-deadline requests reached the scheduler before the operating system ran the first coder thread.

## Limits of black-box testing

The harness cannot prove structural requirements such as:

- absence of mutable global state in the mandatory implementation;
- use of only authorized functions;
- exactly one coder thread per coder;
- presence of a separate monitor thread;
- one mutex protecting each dongle;
- the internal priority-queue implementation.

Those points still require source inspection and the normal evaluator checks.
