# Phase 6 Scheduler Validation

Manual evidence for FIFO/EDF behavior. These commands are not a permanent test harness.

## Commands

```sh
make re
norminette coders

./codexion 1 100 50 50 50 1 0 fifo
./codexion 2 120 200 50 50 1 0 fifo
./codexion 5 1000 80 120 220 3 40 fifo
./codexion 5 1000 80 120 220 3 40 edf
```

## Expected observations

| Scenario | What it proves |
| --- | --- |
| `1 100 50 50 50 1 0 fifo` | One coder cannot compile and burns out once. |
| `2 120 200 50 50 1 0 fifo` | Burnout stops normal logs; the burnout line is last. |
| `5 1000 80 120 220 3 40 fifo` | Viable FIFO run completes without burnout and preserves request-order scheduling. |
| `5 1000 80 120 220 3 40 edf` | Viable EDF run completes without burnout and chooses a different compile order once deadlines diverge. |

## Verified evidence

A temporary WSL script asserted:
- every log line matches the allowed message format;
- one-coder and forced-burnout cases emit exactly one burnout line;
- no normal logs appear after burnout;
- viable FIFO and EDF cases do not burn out;
- every coder reaches the required compile count in viable cases;
- FIFO and EDF compile sequences differ on the selected 5-coder scenario.

Observed compile sequences for the scheduler-difference scenario:

```txt
fifo: 1 2 3 4 5 1 2 3 4 5 1 2 3 4 5
edf:  1 2 3 4 1 5 2 3 4 1 5 2 3 4 1 5 2
```

Defense note: FIFO follows request arrival order. EDF recalculates priority from each coder's `last_compile_start_ms + time_to_burnout`, so coders with earlier burnout deadlines can jump ahead after the first cycle.
