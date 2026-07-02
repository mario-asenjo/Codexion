# Skill: Ponytail Optimizer

Use this only after Codexion behavior is correct and tests are green.

## Rule maximum
Do not optimize performance before correcting behavior. In this project, a slightly simpler and verifiable solution beats a clever solution that is hard to defend.

## Checklist
- Does each function have one responsibility?
- Can every lock be explained?
- Are critical sections too large?
- Is there duplication between FIFO and EDF?
- Is the heap decoupled from scheduling policy?
- Is the parser isolated?
- Is the monitor isolated?
- Is the EDF tie-breaker isolated for recode?
- Do names explain intention?
- Is any code clever but fragile?
- Can code be reduced without breaking the 42 Norm?
- Does it still compile with `-Wall -Wextra -Werror -pthread`?
- Does Valgrind still pass?
- Does Helgrind/DRD still pass if available?

## Output format
When applying Ponytail, report:
1. simplification made;
2. invariant preserved;
3. test re-run;
4. risk reduced.
