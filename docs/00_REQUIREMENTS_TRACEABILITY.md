# 00 — Assignment Requirements and Traceability

**Source of truth:** `reference/PA-PartIII-2026Fall.pdf` (five pages; original uploaded assignment). This matrix is a navigation aid, not an alternative specification. Read the source if a question arises.

## Problem and grading

- **Task A (35 pts):** maximum-profit non-overlapping deliveries on one driver, interval finish may equal next start.
- **Task B (25 pts):** 0/1 selection of deliveries under a fuel budget; scheduling conflicts do not matter.
- **Task C (20 pts):** maximum permitted deletions while preserving `pattern` as a subsequence of `source`, using original zero-based `targetIndices`.
- **Task D (20 pts):** reproducible code, README, tests/test results, short PDF report, packaged ZIP.

| PDF criterion | Points | Precise obligation | Primary design | Ticket | Gate |
|---|---:|---|---|---|---|
| A1 | 8 | Sort by nondecreasing finish; last compatible predecessor `p(i)` with binary search; total preprocessing O(n log n) | `02_TASK_A` | A-01 | G2/G3 |
| A2 | 12 | Weighted interval DP, state/base/recurrence; no greedy/exhaustive main solution | `02_TASK_A` | A-01 | G2/G3 |
| A3 | 8 | Return optimal profit and chronological compatible IDs; any optimum allowed | `02_TASK_A` | A-02 | G2/G3 |
| A4 | 7 | Counterexample to greedy; complexity including sorting, predecessor and reconstruction | `02_TASK_A`, `12_TASK_D` | A-02, DOC-02 | G5 |
| B1 | 5 | Define DP on first i deliveries and budget b, base/recurrence | `03_TASK_B` | B-01 | G2 |
| B2 | 12 | Bottom-up **2D** DP; return optimal profit; empty input and B=0 | `03_TASK_B` | B-01 | G2/G3 |
| B3 | 5 | Reconstruct IDs, total fuel, total profit; no reuse, within budget | `03_TASK_B` | B-02 | G2/G3 |
| B4 | 3 | O(nB) time/space and explain dependence on numeric value B | `03_TASK_B` | DOC-02 | G5 |
| C1 | 5 | **2D** DP on source/pattern prefixes; base/recurrence/fill order/impossible state | `04_TASK_C` | C-01 | G2 |
| C2 | 9 | Bottom-up DP maximizing deletions; match vs skip; delete only eligible indices | `04_TASK_C` | C-01 | G2/G3 |
| C3 | 4 | Return original indices of an optimum and verify preserved subsequence | `04_TASK_C` | C-02 | G2/G3 |
| C4 | 2 | Test repeats/no removals/multiple alignments; O(nm) time/extra space | `04_TASK_C`, `11_TEST_PLAN` | C-02 | G3/G5 |
| D1 | 5 | README: tasks, language/version, dependencies, actual commands for each, tests, I/O, examples | `12_TASK_D`, `13_USABILITY` | DOC-01, UX-01 | G5/G5-U |
| D2 | 8 | Given example + >=3 more per task; input/expected/**actual**; required edges; A/B independent small check; C validity | `11_TEST_PLAN` | TEST-01, DOC-02 | G3/G5 |
| D3 | 5 | **PDF report**: DP state/base/recurrence/fill order/complexity/traceback per task, test refs, real lesson/challenge | `12_TASK_D` | DOC-02 | G5 |
| D4 | 2 | One Canvas ZIP: source, README, tests/results, PDF report, descriptive names/comments; disclose AI and independently verify | `12_TASK_D`, `14_RELEASE_PLAYBOOK` | RELEASE-01 | G5 |

## Requirements vs. elected implementation restrictions

The PDF **does not** specify a CLI, numeric encoding for A time/profit, C++20, CMake, fixed-point money, repository/agent setup, containers, or prebuilt distributions. Those are **user-approved architecture decisions**, not professor requirements. In particular, Part A accepts profit >=0 with no mandated cents precision; our chosen 2-decimal input precision must be documented and tested.

## PDF editorial irregularity

Page 4, after Task C Example 4, contains a stray explanatory paragraph about Example 1 (`"aba"` / `"abaa"`). Do not treat it as an extra test or override Example 4. Compare the formal C1-C4 requirements and the examples themselves.

## Implementation policy

The professor permits standard-library sorting/basic data structures and generative AI with acknowledgement, but **core DP logic must be implemented explicitly and independently verified**. Exhaustive subset enumeration belongs only in small-instance **test oracles**, never in the production algorithm.
