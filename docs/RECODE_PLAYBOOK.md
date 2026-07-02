# Recode Playbook

## Known recode
Modify EDF so that when two coders have the same deadline, the coder with the higher `coder_id` wins.

## Design requirement
EDF tie-breaking must live in one small function, for example:

```c
int edf_tie_break(t_request a, t_request b)
```

The heap comparator should call this function instead of embedding tie-break logic in multiple places.

## Base behavior recommendation
Before recode, choose one clear tie-break policy:
- lower `coder_id` first; or
- lower request sequence first for stability.

Document which one is used.

## Recode steps
1. Open the scheduler comparator file.
2. Locate the EDF tie-break function.
3. Change only equal-deadline behavior to prefer higher `coder_id`.
4. Rebuild with `make re`.
5. Run a small EDF case that creates equal deadlines.
6. Explain that no heap algorithm changed; only comparator policy changed.

## Defense sentence
“The heap is policy-agnostic. EDF ordering is centralized in the comparator, and equal-deadline behavior is isolated in one function, so recode changes scheduling policy without touching thread synchronization or heap mechanics.”
