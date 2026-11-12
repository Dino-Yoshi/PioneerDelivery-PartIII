# Test results

Every value below comes from a command run in the authoring session in the `/work` workspace.
The Expected column comes from `docs/11_TEST_PLAN.md` and the assignment. The ACTUAL column is
the exact `--machine` output of the program. Each row used one CLI run with `--input` and
`--machine`. No ACTUAL value was copied from Expected.

No row reports FAIL. Each optimum matches its expected value, and each printed selection passes
the validity check written in the last column.

## Toolchain and build

| Item | Value | Source |
|---|---|---|
| CMake | 3.31.6 | `cmake --version` |
| C++ compiler | GCC 14.2.0 | `build/CMakeFiles/3.31.6/CMakeCXXCompiler.cmake`, written by CMake at configure time. A direct `c++ --version` call was blocked by a permission rule in this session. |
| Configure | `cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release` | exit 0 |
| Build | `cmake --build build` | exit 0, 16 build steps, no compiler warnings shown in the output |

The validity checks in the tables were done by reading each output and the fixture by hand. The
oracle programs repeat them in code (see the last two sections).

## Task A: weighted interval scheduling

Validity condition: the selected deliveries do not overlap (finish of an earlier delivery is at
most the start of the next), and their profits sum to the printed maximum.

| Test ID | Fixture | Input summary | Expected (optimum; validity) | ACTUAL (machine output) | Exit code | Optimum matches | Selection valid |
|---|---|---|---|---|---:|---|---|
| A-01 | tests/fixtures/A-01.txt | Assignment example, 6 deliveries A to F | 180.00; compatible set | max_profit=180.00 / selected_count=3 / selected_ids=B,D,F | 0 | YES | YES: B [1,4), D [4,7), F [7,9) do not overlap; 60+70+50 = 180.00 |
| A-02 | tests/fixtures/A-02.txt | Zero deliveries | 0.00; empty ID list | max_profit=0.00 / selected_count=0 / selected_ids= | 0 | YES | YES: empty selection, sum 0.00 |
| A-03 | tests/fixtures/A-03.txt | A [0,2) 30.00, B [2,5) 40.00 (endpoint equality) | 70.00; compatible | max_profit=70.00 / selected_count=2 / selected_ids=A,B | 0 | YES | YES: A finishes at 2 and B starts at 2, which is allowed; 30+40 = 70.00 |
| A-04 | tests/fixtures/A-04.txt | A [0,3) 50.00, B [1,4) 60.00, C [2,5) 40.00 (all overlap) | 60.00; one delivery | max_profit=60.00 / selected_count=1 / selected_ids=B | 0 | YES | YES: a single delivery; 60.00 |
| A-05 | tests/fixtures/A-05.txt | Two identical [0,2) deliveries, 50.00 each | 50.00; one delivery (either ID) | max_profit=50.00 / selected_count=1 / selected_ids=A | 0 | YES | YES: one of the two tied deliveries; 50.00 |
| A-06 | tests/fixtures/A-06.txt | Greedy counterexample: A [0,2) 10.00, B [0,3) 100.00, C [2,4) 10.00 | 100.00; B alone | max_profit=100.00 / selected_count=1 / selected_ids=B | 0 | YES | YES: a single delivery; 100.00 (earliest-finish greedy would give 20.00) |
| A-07 | tests/fixtures/A-07.txt | Three deliveries, all profit 0.00 | 0.00; empty ID list | max_profit=0.00 / selected_count=0 / selected_ids= | 0 | YES | YES: empty selection, sum 0.00 |
| A-08 | tests/fixtures/A-08.txt | Negative times, equal finish values (Q and R finish at -2), shuffled input | 55.00; compatible set | max_profit=55.00 / selected_count=2 / selected_ids=Q,S | 0 | YES | YES: Q [-4,-2) and S [-2,0) touch at -2; 25.00+30.00 = 55.00. P,R,S also reaches 55.00, so another optimum exists. |

## Task B: 0/1 knapsack with a fuel budget

Validity condition: each ID appears once, total fuel of the selected deliveries is at most the
budget, and their profits sum to the printed maximum.

| Test ID | Fixture | Input summary | Expected (optimum; validity) | ACTUAL (machine output) | Exit code | Optimum matches | Selection valid |
|---|---|---|---|---|---:|---|---|
| B-01 | tests/fixtures/B-01.txt | Assignment example, budget 10, deliveries P to T | 53; fuel 10 | max_profit=53 / total_fuel=10 / selected_count=3 / selected_ids=Q,R,S | 0 | YES | YES: fuel 3+5+2 = 10 <= 10; profit 14+29+10 = 53 |
| B-02 | tests/fixtures/B-02.txt | No deliveries, budget 10 | 0; empty | max_profit=0 / total_fuel=0 / selected_count=0 / selected_ids= | 0 | YES | YES: empty selection |
| B-03 | tests/fixtures/B-03.txt | Two deliveries with cost 2 and 3, budget 0 | 0; empty | max_profit=0 / total_fuel=0 / selected_count=0 / selected_ids= | 0 | YES | YES: no delivery fits in 0 fuel |
| B-04 | tests/fixtures/B-04.txt | A (fuel 4, profit 20), B (fuel 5, profit 21), budget 4 | 20; A alone | max_profit=20 / total_fuel=4 / selected_count=1 / selected_ids=A | 0 | YES | YES: fuel 4 <= 4 (exact fit); profit 20 |
| B-05 | tests/fixtures/B-05.txt | P (6, 30), Q (5, 24), R (5, 24), budget 10 | 48; Q and R | max_profit=48 / total_fuel=10 / selected_count=2 / selected_ids=Q,R | 0 | YES | YES: fuel 5+5 = 10 <= 10; profit 24+24 = 48 |
| B-06 | tests/fixtures/B-06.txt | A (2, 5), B (3, 5), budget 3 (profit tie) | 5; one delivery | max_profit=5 / total_fuel=2 / selected_count=1 / selected_ids=A | 0 | YES | YES: fuel 2 <= 3; profit 5. B alone also gives 5. |
| B-07 | tests/fixtures/B-07.txt | A (cost 5), B (cost 7), budget 4; every delivery costs more than the budget | 0; empty | max_profit=0 / total_fuel=0 / selected_count=0 / selected_ids= | 0 | YES | YES: empty selection |
| B-08 | tests/fixtures/B-08.txt | A (3, 9), B (2, 6), budget 4 | 9; A alone | max_profit=9 / total_fuel=3 / selected_count=1 / selected_ids=A | 0 | YES | YES: fuel 3 <= 4; profit 9. One unit of budget stays unused. |

## Task C: event-log cleanup

Validity condition: each removed index is eligible (in the case's index list), indices are
strictly ascending, and the source with those positions deleted still contains the pattern as a
subsequence. The check was done by hand on the kept string.

| Test ID | Fixture | Input summary (source / pattern / eligible indices) | Expected (optimum; validity) | ACTUAL (machine output) | Exit code | Optimum matches | Selection valid |
|---|---|---|---|---|---:|---|---|
| C-01 | tests/fixtures/C-01.txt | `abbaa` / `aba` / `[0,1,2]` (assignment example 1) | 1; kept string contains `aba` | max_deletions=1 / removed_count=1 / removed_indices=2 | 0 | YES | YES: kept `abba` contains `aba` |
| C-02 | tests/fixtures/C-02.txt | `bcda` / `d` / `[0,3]` (assignment example 2) | 2; kept string contains `d` | max_deletions=2 / removed_count=2 / removed_indices=0,3 | 0 | YES | YES: kept `cd` contains `d` |
| C-03 | tests/fixtures/C-03.txt | `dda` / `dda` / `[0,1,2]` (assignment example 3) | 0; nothing removed | max_deletions=0 / removed_count=0 / removed_indices= | 0 | YES | YES: nothing removed, `dda` intact |
| C-04 | tests/fixtures/C-04.txt | `yeyeykyded` / `yeyyd` / `[0,2,3,4]` (assignment example 4) | 2; kept string contains `yeyyd` | max_deletions=2 / removed_count=2 / removed_indices=3,4 | 0 | YES | YES: kept `yeykyded` contains `yeyyd` (y0 e1 y2 y6 d7). The assignment's deletion set 2,3 is also valid. |
| C-05 | tests/fixtures/C-05.txt | `abaaba` / `aba` / `[0,1,2]` (greedy earliest alignment fails) | 3; kept string contains `aba` | max_deletions=3 / removed_count=3 / removed_indices=0,1,2 | 0 | YES | YES: kept `aba` (positions 3 to 5) |
| C-06 | tests/fixtures/C-06.txt | `abc` / `ac` / no eligible indices | 0; nothing removed | max_deletions=0 / removed_count=0 / removed_indices= | 0 | YES | YES: nothing removed |
| C-07 | tests/fixtures/C-07.txt | `aaaa` / `aa` / `[0,1,2,3]` (repeated symbols) | 2; kept string contains `aa` | max_deletions=2 / removed_count=2 / removed_indices=2,3 | 0 | YES | YES: kept `aa` |
| C-08 | tests/fixtures/C-08.txt | `xyzabc` / `abc` / `[0,1,2]` | 3; kept string contains `abc` | max_deletions=3 / removed_count=3 / removed_indices=0,1,2 | 0 | YES | YES: kept `abc` |
| C-09 | tests/fixtures/C-09.txt | `xabcy` / `ac` / `[0,4]` (boundary characters) | 2; kept string contains `ac` | max_deletions=2 / removed_count=2 / removed_indices=0,4 | 0 | YES | YES: kept `abc` contains `ac` |
| C-10 | tests/fixtures/C-10.txt | `abc` / `abc` / `[0,1,2]` (no deletion possible) | 0; nothing removed | max_deletions=0 / removed_count=0 / removed_indices= | 0 | YES | YES: nothing removed, `abc` intact |

## Invalid and limit inputs

Each command ran once with `--input` and `--machine`. Stdout was empty in every run, and the
first line below is the only line the program printed (on stderr).

| Fixture | Command | Exit code | First stderr line |
|---|---|---:|---|
| tests/fixtures/invalid-schedule-syntax.txt | `build/pioneer schedule --input tests/fixtures/invalid-schedule-syntax.txt --machine` | 2 | `schedule: record 2: finish must be a signed 64-bit decimal integer, got 'four'` |
| tests/fixtures/invalid-knapsack-syntax.txt | `build/pioneer knapsack --input tests/fixtures/invalid-knapsack-syntax.txt --machine` | 2 | `knapsack: record 2: missing profit` |
| tests/fixtures/invalid-cleanup-syntax.txt | `build/pioneer cleanup --input tests/fixtures/invalid-cleanup-syntax.txt --machine` | 2 | `cleanup: count k must be a decimal integer of at least 0, got 'three'` |
| tests/fixtures/invalid-schedule-duplicate-id.txt | `build/pioneer schedule --input tests/fixtures/invalid-schedule-duplicate-id.txt --machine` | 2 | `schedule: record 2: duplicate id 'A'` |
| tests/fixtures/invalid-cleanup-index-order.txt | `build/pioneer cleanup --input tests/fixtures/invalid-cleanup-index-order.txt --machine` | 2 | `cleanup: target index 1 must be greater than the previous target index 2` |
| tests/fixtures/invalid-schedule-start-not-before-finish.txt | `build/pioneer schedule --input tests/fixtures/invalid-schedule-start-not-before-finish.txt --machine` | 2 | `schedule: record 1: finish must be greater than start` |
| tests/fixtures/invalid-knapsack-zero-fuel.txt | `build/pioneer knapsack --input tests/fixtures/invalid-knapsack-zero-fuel.txt --machine` | 2 | `knapsack: record 1: fuel cost must be a decimal integer of at least 1, got '0'` |
| tests/fixtures/invalid-cleanup-not-subsequence.txt | `build/pioneer cleanup --input tests/fixtures/invalid-cleanup-not-subsequence.txt --machine` | 2 | `cleanup: pattern is not a subsequence of source` |
| tests/fixtures/limit-knapsack-table.txt | `build/pioneer knapsack --input tests/fixtures/limit-knapsack-table.txt --machine` | 1 | `pioneer: knapsack table: (n+1)=2 by (B+1)=1000000000000 is 2000000000000 cells, which exceeds the limit of 268435456 bytes (33554432 cells)` |

The limit fixture is a file, not a stdin pipe. Its content is `1 999999999999` followed by
`A 1 1`: one delivery with budget 999999999999. It was written for this check and is not from
the assignment.

Human-readable mode, one run per task (exit code 0 each):

| Command | Output |
|---|---|
| `build/pioneer schedule --input tests/fixtures/A-01.txt` | `Weighted interval scheduling` / `Maximum profit: 180.00` / `Selected deliveries (3): B, D, F` |
| `build/pioneer knapsack --input tests/fixtures/B-01.txt` | `Delivery selection with a fuel budget` / `Maximum profit: 53` / `Total fuel used: 10` / `Selected deliveries (3): Q, R, S` |
| `build/pioneer cleanup --input tests/fixtures/C-01.txt` | `Event log cleanup` / `Maximum deletions: 1` / `Removed original indices (1): 2` |

## CTest

Command: `ctest --test-dir build --output-on-failure`. Exit code 0.

| # | Test name | Result | Time |
|---:|---|---|---:|
| 1 | SchedulingTests | Passed | 0.01 sec |
| 2 | KnapsackTests | Passed | 0.11 sec |
| 3 | CleanupTests | Passed | 0.02 sec |
| 4 | SchedulingOracleTests | Passed | 0.00 sec |
| 5 | KnapsackOracleTests | Passed | 0.00 sec |
| 6 | CleanupOracleTests | Passed | 0.00 sec |
| 7 | parser_tests | Passed | 0.00 sec |
| 8 | formatter_tests | Passed | 0.00 sec |
| 9 | cli_checks | Passed | 0.08 sec |

Total: 100% tests passed, 0 tests failed out of 9. Total test time 0.23 sec.

## Unit test counts

These are the counts printed by each executable when run directly, with exit code 0.

| Executable | Output | Exit code |
|---|---|---:|
| build/SchedulingTests | `SchedulingTests: 1878 of 1878 checks passed` | 0 |
| build/KnapsackTests | `at-limit table test (budget 16777215, n=1, 268435456 bytes): 0.13218 s` then `knapsack_tests: 2181 of 2181 checks passed` | 0 |
| build/CleanupTests | `sweep: 15932 generated cases checked` then `cleanup_tests: 95741 of 95741 checks passed` | 0 |

## Oracle programs and seeds

Each oracle program has two phases. Phase 1 checks the brute-force oracle against the expected
values of the named cases. Phase 2 compares the production solver with the oracle on the named
cases and on the seeded random instances for seeds 1 to 25, and checks the reconstruction
invariants. Each instance gives two phase 2 checks: the optimum, and the validity invariant.
For A and B that is 8 named cases plus 25 seeds (33 instances, 66 checks). For C it is 10 named
cases plus 25 seeds (35 instances, 70 checks).

| Executable | Phase 1 output | Phase 2 output | Exit code |
|---|---|---|---:|
| build/SchedulingOracleTests | `ORACLE SELF-CHECKS: 8 of 8 checks passed` | `PRODUCTION COMPARISON: 66 of 66 checks passed` | 0 |
| build/KnapsackOracleTests | `ORACLE SELF-CHECKS: 8 of 8 checks passed` | `PRODUCTION COMPARISON: 66 of 66 checks passed` | 0 |
| build/CleanupOracleTests | `ORACLE SELF-CHECKS: 10 of 10 checks passed` | `PRODUCTION COMPARISON: 70 of 70 checks passed` | 0 |

The oracles enumerate subsets: all masks for A and B (n at most 10), and all subsets of eligible
indices for C (at most 12). They were written before the solvers were merged, and the worker
that wrote them never opened the solver sources. The oracle code uses no DP table, predecessor
array or traceback.

## Known failures

None. No actual value differs from its expected value, and no test failed.
