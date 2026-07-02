# Skill: Timing and Monitoring

Use this when implementing timestamps, sleeps, cooldown, or burnout detection.

## Time model
- Use `gettimeofday`.
- Convert to milliseconds.
- Store `sim.start_ms` once.
- Logs use `now_ms - sim.start_ms`.

## Burnout
Deadline is `last_compile_start_ms + time_to_burnout`.
Monitor must detect and log burnout within 10 ms under reasonable evaluation timings.

## Waiting
- Avoid aggressive busy-wait.
- Prefer predicate loops with condition waits and timed waits.
- Cooldown expires due to time, not only broadcasts, so waiters must re-check on timeouts.

## Pitfalls
- Updating `last_compile_start_ms` after logging compile instead of at compile start.
- Sleeping a full action duration without checking stop can delay shutdown logs.
- Comparing absolute and relative timestamps incorrectly.
