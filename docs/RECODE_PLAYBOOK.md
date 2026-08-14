# Recode Playbook

## Known recode
Modify EDF so that when two coders have the same deadline, the coder with the higher `coder_id` wins.

## Design requirement
EDF tie-breaking must live in one small function:

```c
int cx_edf_tie_break(t_request a, t_request b)
```

The heap comparator calls this function instead of embedding tie-break logic in multiple places.

## Base behavior
The submitted implementation prefers the lower `coder_id` when EDF deadlines are equal, with request sequence as a final deterministic fallback.

## Recode steps
1. Open `coders/heap_order.c`.
2. Locate `cx_edf_tie_break`.
3. Change only equal-deadline behavior to prefer higher `coder_id`.
4. Rebuild with `make re`.
5. Run a small EDF case that creates equal deadlines and contention on the same dongle.
6. Explain that no heap algorithm or synchronization rule changed; only comparator policy changed.

The evaluator change is conceptually:

```diff
- return (a.coder_id < b.coder_id);
+ return (a.coder_id > b.coder_id);
```

EDF first compares `deadline_ms`; only equal deadlines call `cx_edf_tie_break`.

## Defense sentence
“The heap mechanics are policy-agnostic. EDF ordering is centralized in the comparator, and equal-deadline behavior is isolated in one function, so the recode changes scheduling policy without touching resource synchronization or heap mechanics.”
