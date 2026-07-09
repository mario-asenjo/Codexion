# Phase 7 Audit Report

Phase 7 is an evaluator-style audit. No simulator code changed.

## Commands run

```sh
make re
norminette coders
! git grep -n -E '(^|[^A-Za-z0-9_])(calloc|realloc|strtol|isdigit|perror|strerror|exit|pthread_detach|pthread_cond_signal|sem_open|sem_wait|sem_post|nanosleep|clock_gettime|sleep|sprintf|snprintf|qsort)[[:space:]]*\(' -- coders Makefile
valgrind --error-exitcode=42 --leak-check=full --show-leak-kinds=all ./codexion 5 1000 80 120 220 1 40 edf
valgrind --tool=helgrind --error-exitcode=43 ./codexion 5 1000 80 120 220 1 40 edf
./codexion 2 120 200 50 50 1 0 fifo
make
```

## Results

- Build: clean.
- Norminette: all `coders` files OK.
- Forbidden function scan: clean.
- Valgrind Memcheck: `All heap blocks were freed -- no leaks are possible`; `ERROR SUMMARY: 0 errors from 0 contexts`.
- Helgrind: `ERROR SUMMARY: 0 errors from 0 contexts`.
- Burnout smoke: one burnout line and no normal logs after stop.
- No relink: `make: Nothing to be done for 'all'.`

## Manual lock-order review

Observed lock nesting follows the project contract:

```txt
state_lock -> log_lock
```

No code path takes `log_lock` before `state_lock`. Dongle mutexes are initialized/destroyed but not used in current arbitration; ownership and cooldown are guarded by `state_lock`.

## Remaining follow-up

README/defense and recode rehearsal remain later phases.
