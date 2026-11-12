# Test results

**Status: PENDING.** No production result has been recorded. The table below lists the named
cases from `docs/11_TEST_PLAN.md`. The Expected column is copied from the plan. The ACTUAL,
exit code, and PASS/FAIL columns are filled only from real runs. Until the solvers are merged,
every such cell stays `PENDING`.

The oracle programs (`SchedulingOracleTests`, `KnapsackOracleTests`, `CleanupOracleTests`)
also run randomized seeds 1 to 25 per task. Their outcome is in the last section.

## Task A: weighted interval scheduling (maximum profit, cents)

| Case ID | Input summary | Expected | ACTUAL | Exit code | PASS/FAIL |
|---|---|---:|---|---|---|
| A-01 | Assignment example, deliveries A to F | 180.00 | PENDING | PENDING | PENDING |
| A-02 | Zero deliveries | 0.00 | PENDING | PENDING | PENDING |
| A-03 | A `[0,2]` 30.00, B `[2,5]` 40.00 (endpoint equality) | 70.00 | PENDING | PENDING | PENDING |
| A-04 | A `[0,3]` 50.00, B `[1,4]` 60.00, C `[2,5]` 40.00 (all overlap) | 60.00 | PENDING | PENDING | PENDING |
| A-05 | Two equal `[0,2]` deliveries, 50.00 each | 50.00 | PENDING | PENDING | PENDING |
| A-06 | Greedy counterexample: A `[0,2]` 10.00, B `[0,3]` 100.00, C `[2,4]` 10.00 | 100.00 | PENDING | PENDING | PENDING |
| A-07 | Three deliveries, all with zero profit | 0.00 | PENDING | PENDING | PENDING |
| A-08 | Negative times, equal finish values, shuffled input (five deliveries) | Oracle-derived, 55.00 by hand | PENDING | PENDING | PENDING |

## Task B: 0/1 knapsack (maximum profit)

| Case ID | Input summary | Expected | ACTUAL | Exit code | PASS/FAIL |
|---|---|---:|---|---|---|
| B-01 | Assignment example, budget 10, deliveries P to T | 53 | PENDING | PENDING | PENDING |
| B-02 | No deliveries, budget 10 | 0 | PENDING | PENDING | PENDING |
| B-03 | Two deliveries with positive cost, budget 0 | 0 | PENDING | PENDING | PENDING |
| B-04 | A (4, 20), B (5, 21), budget 4 (exact fit) | 20 | PENDING | PENDING | PENDING |
| B-05 | P (6, 30), Q (5, 24), R (5, 24), budget 10 (ratio greedy fails) | 48 | PENDING | PENDING | PENDING |
| B-06 | A (2, 5), B (3, 5), budget 3 (profit tie) | 5 | PENDING | PENDING | PENDING |
| B-07 | Every delivery costs more than the budget | 0 | PENDING | PENDING | PENDING |
| B-08 | A (3, 9), B (2, 6), budget 4 (unused budget) | 9 | PENDING | PENDING | PENDING |

## Task C: event-log cleanup (maximum deletions)

| Case ID | Input summary (source / pattern / eligible indices) | Expected | ACTUAL | Exit code | PASS/FAIL |
|---|---|---:|---|---|---|
| C-01 | `abbaa` / `aba` / `[0,1,2]` (assignment example 1) | 1 | PENDING | PENDING | PENDING |
| C-02 | `bcda` / `d` / `[0,3]` (assignment example 2) | 2 | PENDING | PENDING | PENDING |
| C-03 | `dda` / `dda` / `[0,1,2]` (assignment example 3) | 0 | PENDING | PENDING | PENDING |
| C-04 | `yeyeykyded` / `yeyyd` / `[0,2,3,4]` (assignment example 4) | 2 | PENDING | PENDING | PENDING |
| C-05 | `abaaba` / `aba` / `[0,1,2]` (greedy alignment fails) | 3 | PENDING | PENDING | PENDING |
| C-06 | `abc` / `ac` / no eligible indices | 0 | PENDING | PENDING | PENDING |
| C-07 | `aaaa` / `aa` / `[0,1,2,3]` (repeated symbols) | 2 | PENDING | PENDING | PENDING |
| C-08 | `xyzabc` / `abc` / `[0,1,2]` | 3 | PENDING | PENDING | PENDING |
| C-09 | `xabcy` / `ac` / `[0,4]` (boundary characters) | 2 | PENDING | PENDING | PENDING |
| C-10 | `abc` / `abc` / `[0,1,2]` (no deletion possible) | 0 | PENDING | PENDING | PENDING |

## Randomized oracle runs

The seeds 1 to 25 were not reached. Phase 2 stops at the first production call, which throws
`not implemented`, so the random instances were never compared with a solver result. The
columns below record the real output of the oracle program runs in this session.

| Program | Seeds | Phase 1 (oracle self-checks) | Phase 2 (production comparison) | Exit code | Result |
|---|---|---|---|---|---|
| SchedulingOracleTests | 1 to 25 | 8 of 8 checks passed | 0 of 1 checks passed; `production call threw: Scheduling: not implemented` | 1 | FAIL (stub) |
| KnapsackOracleTests | 1 to 25 | 8 of 8 checks passed | 0 of 1 checks passed; `production call threw: Knapsack: not implemented` | 1 | FAIL (stub) |
| CleanupOracleTests | 1 to 25 | 10 of 10 checks passed | 0 of 1 checks passed; `production call threw: Cleanup: not implemented` | 1 | FAIL (stub) |
