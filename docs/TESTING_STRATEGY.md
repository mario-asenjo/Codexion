# Testing Strategy

## Build commands

```sh
make
make re
```

Both must be clean: no compiler warnings and no unnecessary relink.

## Base runtime commands

```sh
./codexion 1 200 100 100 100 1 60 fifo
./codexion 2 800 100 100 100 3 60 fifo
./codexion 5 800 100 100 100 3 60 edf
./codexion 10 1000 80 80 80 2 60 edf
```

## Easy evaluation
- Do not test above 200 coders for expected evaluator scope.
- Do not rely on timings under 60 ms.
- Verify viable parameters do not burn out.
- Verify all coders complete required compiles.
- Verify output lines match the exact allowed messages.

## Less easy evaluation
- Burnout edge cases.
- One coder case.
- Tight timing cases around deadline.
- Stop coordination: no normal logs after burnout.
- No duplicate dongle ownership.
- Correct state transitions: compile, debug, refactor.

## Medium evaluation
- Cooldown has observable effect.
- FIFO and EDF can produce different scheduling choices.
- Refactoring timing is respected.
- Logs are serialized even under many coders.

## Invalid input tests
```sh
./codexion
./codexion 5 800 100 100 100 3 60 bad
./codexion -5 800 100 100 100 3 60 fifo
./codexion 5 800x 100 100 100 3 60 fifo
./codexion 5 800 100 100 100 3 60 fifo extra
```

## Memory

```sh
valgrind --leak-check=full --show-leak-kinds=all ./codexion 5 800 100 100 100 3 60 edf
```

## Races

```sh
valgrind --tool=helgrind ./codexion 5 800 100 100 100 3 60 edf
```

If Valgrind is unavailable on the host, document that limitation and run the strongest available alternative. Do not weaken the code because the tool is unavailable.

## Forbidden function scan
Use source scans as a first pass only. Final confidence comes from review and compilation:

```sh
grep -R "calloc\|realloc\|strtol\|isdigit\|perror\|strerror\|exit\|pthread_detach\|pthread_cond_signal\|sem_open\|sem_wait\|sem_post\|nanosleep\|clock_gettime\|sleep\|sprintf\|snprintf\|qsort" coders Makefile
```

Tests must not become an excuse to include forbidden functions in the final binary.
