# Skill: Scheduler Heap FIFO/EDF

Use this when implementing or reviewing the custom priority heap.

## Heap responsibilities
- Store pending compile requests.
- Support push, pop, peek, remove/update if needed.
- Never depend on `qsort` or library priority queues.
- Keep heap mechanics separate from scheduler policy.

## Comparator responsibilities
FIFO:
- earlier request sequence wins.

EDF:
- earlier deadline wins;
- equal deadline delegates to an isolated tie-break function;
- stable fallback should be explicit.

## Recode guard
The equal-deadline EDF behavior must be changeable in one small function. Do not spread tie-break rules across heap operations.

## Tests to design
- Three FIFO requests inserted out of coder order.
- Three EDF requests with different deadlines.
- Equal-deadline EDF requests to prove the tie-breaker.
- Heap remains valid after repeated push/pop.
