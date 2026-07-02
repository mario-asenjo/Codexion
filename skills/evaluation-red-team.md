# Skill: Evaluation Red Team

Use this before claiming a phase or final project is ready.

## Attack surface
- Compiler warnings.
- Forbidden functions.
- Data races.
- Deadlocks.
- Mixed logs.
- Leaks.
- Incorrect one-coder behavior.
- Burnout timing over 10 ms.
- Scheduler priority not actually enforced.
- Cooldown not actually enforced.
- Normal logs after stop.

## Method
1. Re-read the spec distilled file.
2. Run build and minimal tests.
3. Scan for forbidden functions and globals.
4. Run memory/race tools if available.
5. Try edge cases that should fail parser.
6. Try cases that force burnout.
7. Try cases that must complete.

## Final question
If a strict evaluator tried to give this project zero, what would they point at first?
