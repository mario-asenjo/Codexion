# Skill: POSIX Pthreads Concurrency

Use this when designing or reviewing thread synchronization in Codexion.

## Invariants
- Shared state is read or written only under the correct mutex.
- Logs are serialized under `log_lock`.
- Stop flag is protected by `state_lock`.
- Condition variable waits happen in loops that re-check predicates.
- `pthread_cond_broadcast` is used because `pthread_cond_signal` is forbidden.

## Lock order
1. `state_lock`
2. Optional `dongle.lock`
3. `log_lock`

Never take `state_lock` after `log_lock`.

## Red flags
- Holding one dongle while waiting indefinitely for another.
- Reading `stop`, `last_compile_start_ms`, or counters without a lock.
- Logging while another lock order path can invert locks.
- Condvar wait without predicate loop.
- Cleanup that destroys mutexes while threads may still use them.
