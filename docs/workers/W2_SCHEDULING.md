# W2 — Weighted Interval Scheduling Worker Brief

**Worker:** Kiro CLI / Claude Haiku 5.5 Medium, isolated `feature/scheduling` worktree/container. **Tickets:** `A-01`, `A-02`. **Start only after G1 interface freeze.**

## Problem

One driver; deliveries `(id, start, finish, profitCents)`. Select maximum-profit non-overlapping subset. `finish == next.start` is compatible. PDF Task A requires nondecreasing finish sorting, **binary-search** predecessor `p(i)`, DP and reconstruction, total **O(n log n)** time and O(n) auxiliary DP/predecessor storage (sorting aside).

## State and logic

1-based mathematical recurrence: `OPT[0]=0`, `OPT[i]=max(OPT[i-1], w_i+OPT[p(i)])`, `p(i)=max earlier j with finish[j]<=start[i]` or 0. Input uses signed int64 time, exact int64 cents. Use checked additions.

Traceback excludes on equal inclusion/exclusion profit; otherwise selects delivery i and jumps to `p(i)`. Reverse selected IDs; they must be chronological, compatible and their cents sum must equal optimum.

## Allowed files

`include/pioneer/Scheduling.hpp`, `src/Scheduling.cpp`, `tests/SchedulingTests.cpp`, related task-local fixtures as granted by orchestrator. Treat shared header signatures from G1 as fixed; request changes through Sonnet.

## Required test coverage

Official A..F returns optimum `180.00`; empty set; all overlapping; endpoint equality; zero profits; equal finish/ties; greedy counterexample A(0,2,10), B(0,3,100), C(2,4,10) returns 100. Check invalid overflow behavior where algorithm owns it. W5 independently verifies small-instance enumeration.

## Output to orchestrator

Changed-file list, recurrence/traceback summary, actual tested commands/results, complexity rationale, any constraint interpretation, outstanding issues, worker checkpoint reference. **No commit/merge/push and no CLI/CMake edits.**
