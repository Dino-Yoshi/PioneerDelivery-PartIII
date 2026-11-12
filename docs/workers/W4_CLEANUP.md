# W4 — Subsequence Cleanup Worker Brief

**Worker:** Kiro CLI / Claude Haiku 5.5 Medium, isolated `feature/cleanup` worktree/container. **Tickets:** `C-01`, `C-02`. **Start after G1.** This task receives extra Sonnet review because greedy alignment is deceptively plausible.

## Problem

Lowercase nonempty `source` and `pattern`; pattern initially a subsequence of source. Eligible original zero-based indices are sorted and unique. Maximize count of deletions from eligible positions while keeping pattern as subsequence; report optimum and one ascending optimal list of **original** indices. Index positions never shift.

## Required 2D DP

`F[i][j]` = maximum deletions after first i source chars while matching exactly first j pattern chars. `F[0][0]=0`, `F[0][j>0]=unreachable`, `F[i][0]=eligible count in first i`. For source index i-1:

- Skip: from `F[i-1][j] + (eligible?1:0)` if reachable; leave ineligible skipped chars in source.
- Match: from `F[i-1][j-1]` only if chars equal and previous state reachable.
- Take maximum; never treat impossible states as zero.

Time/space `O(n*m)` for 2D table and reconstruction. Guard allocation. Start traceback from `(n,m)`; favor valid skip transition on score ties; record only eligible skipped original index `i-1`, otherwise take matching transition, reverse list to ascending. Validate deleting exactly those indices still leaves pattern as subsequence.

## Allowed files

`include/pioneer/Cleanup.hpp`, `src/Cleanup.cpp`, `tests/CleanupTests.cpp`, related fixtures. No CLI/CMake/shared-header edits without orchestrator request.

## Required tests

All **four** official examples: outputs 1, 2, 0, 2. Repeated characters/multiple optimal alignments, no eligible positions, all eligible removable, original-index semantics, and `source="abaaba",pattern="aba",indices=[0,1,2]` optimum **3**. W5 separately enumerates small deletion subsets and checks kept strings.

## Deliverable

DP and traceback implementation, independent kept-string check in tests, actual test commands/results, human-readable recurrence comments, complexity note, checkpoint. **Do not commit/merge/push.**
