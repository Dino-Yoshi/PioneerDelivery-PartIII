# 11 — Test Plan, Oracle Design, and Result Recording

This is a planned matrix. **No test below has been executed.** Actual output and PASS/FAIL must be recorded in the eventual `docs/TestResults.md`; never prefill actual data from expected values.

## Universal test assertions

- Algorithm returns the correct objective **and** reconstructs an admissible selected set/index list.
- Stable deterministic tie choice in our implementation, but the grader's examples permit different optima; don't overconstrain test IDs when alternatives exist.
- For every returned ID/index: uniqueness, origin in input, correct ordering, correct budget/nonoverlap/subsequence invariant.
- Human and `--machine` CLI agree semantically; parser failure returns nonzero and meaningful stderr.
- On small n, compare to independent exhaustive reference enumerations; use fixed random seeds and print seed/case on failure.

## A — scheduling planned cases

| Test | Input summary | Expected optimum | Extra verification |
|---|---|---:|---|
| A-01 | PDF A..F example | 180.00 | B,D,F is one valid answer |
| A-02 | `0` deliveries | 0.00 | Empty ID list |
| A-03 | A `[0,2]` 30, B `[2,5]` 40 | 70.00 | Endpoint equality |
| A-04 | A `[0,3]` 50, B `[1,4]` 60, C `[2,5]` 40 | 60.00 | All overlap |
| A-05 | Two equal `[0,2]` deliveries each 50 | 50.00 | Tie, one delivery |
| A-06 | Greedy counterexample A `[0,2]` 10, B `[0,3]` 100, C `[2,4]` 10 | 100.00 | Greedy fails |
| A-07 | All-profit-zero deliveries | 0.00 | No need to choose zero-profit items |
| A-08 | Negative timestamps, equal finish values, shuffled input | Oracle-derived | Binary-search bounds |

Run invalid-parse and overflow tests independently (duplicate IDs, `start>=finish`, >2 profit decimal digits, negative profit, timestamp out of range, cumulative cents overflow).

## B — knapsack planned cases

| Test | Input summary | Expected optimum | Extra verification |
|---|---|---:|---|
| B-01 | PDF P..T, budget 10 | 53 | Q,R,S is one valid answer; fuel 10 |
| B-02 | No items, budget 10 | 0 | Empty result |
| B-03 | Items with positive cost, budget 0 | 0 | No item selected |
| B-04 | A cost 4/profit 20; B cost 5/profit 21; budget 4 | 20 | Exact fit of A |
| B-05 | P(6,30), Q(5,24), R(5,24), budget 10 | 48 | Profit/fuel greedy fails |
| B-06 | A(2,5), B(3,5), budget 3 | 5 | Profit tie, deterministic exclusion |
| B-07 | Every item costs more than budget | 0 | Empty result |
| B-08 | Budget 4; A(3,9), B(2,6) | 9 | Optimal budget may remain unused |

Invalid cases: zero or negative fuel cost, negative profit/budget, duplicate IDs, huge DP table allocation, addition overflow. All selected IDs unique, sum(cost)<=budget, sum(profit)==returned optimum.

## C — cleanup planned cases

| Test | source / pattern | eligible indices | Expected max |
|---|---|---|---:|
| C-01 | `abbaa` / `aba` | `[0,1,2]` | 1 |
| C-02 | `bcda` / `d` | `[0,3]` | 2 |
| C-03 | `dda` / `dda` | `[0,1,2]` | 0 |
| C-04 | `yeyeykyded` / `yeyyd` | `[0,2,3,4]` | 2 |
| C-05 | `abaaba` / `aba` | `[0,1,2]` | 3 |
| C-06 | `abc` / `ac` | `[]` | 0 |
| C-07 | `aaaa` / `aa` | `[0,1,2,3]` | 2 |
| C-08 | `xyzabc` / `abc` | `[0,1,2]` | 3 |
| C-09 | `xabcy` / `ac` | `[0,4]` | 2 |
| C-10 | `abc` / `abc` | `[0,1,2]` | 0 |

**Important:** C-09 ensures eligible boundary characters are deleted while necessary ineligible characters may remain. C-05 demonstrates greedy earliest alignment failure. C-01 has multiple valid index sets; validate by kept-string subsequence rather than exact set.

Invalid cases: source/pattern empty or uppercase, initial pattern not subsequence, negative/unordered/duplicate/out-of-range target indices, k not matching tokens, extra tokens, table size overflow.

## Independent oracle strategies

### A oracle (test-only, small n <=10)
Enumerate each subset of n delivery IDs. Sort that subset by start/finish, reject if adjacent intervals overlap (`finish>next.start`), compute summed profits in cents, maximize. This reference should **not use** production `p(i)`, sorted DP, or reconstruction code.

### B oracle (test-only, small n <=10)
Enumerate 2^n inclusion masks, sum fuel and profit, discard masks over B, maximize profit. Do not reuse DP recurrence or selection traceback.

### C oracle (test-only, small eligible count <=12)
Enumerate subsets of eligible original indices, construct remaining string by one pass over source, use independent two-pointer subsequence predicate, maximize number removed. Do not clone the production prefix DP.

### Deterministic randomized property tests
Generate bounded small inputs using explicitly fixed seeds; print the seed and full input when any mismatch occurs. Generate only valid source/pattern pairs, e.g. choose source then derive a pattern from a random subsequence. Include empty eligible-index lists and dense eligible lists. Random tests augment, not replace, documented named cases.

## Evidence schema

Use one row per named case with stable ID, input fixture path or verbatim input, expected objective/validity condition, **actual** objective/IDs/indices, elapsed time if useful, exit code, PASS/FAIL and CTest test name/log. Because multiple optimal solutions are legal, record accepted validity independently of any professor example's chosen IDs. Reference successful verification in the PDF report, not raw transcript dumps.

## Test quantity and honesty

The professor requires at least **four tests per task**, including the given example; for C, the assignment lists four examples, all of which should be included. This plan exceeds the minimum. Actual test totals and pass counts are not known until execution.
