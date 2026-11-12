# Pioneer Delivery: dynamic programming tools

**Status:** The command-line interface and the test harnesses build. The three solvers
(`solveScheduling`, `solveKnapsack`, `solveCleanup`) are still stubs that throw
`not implemented`, so the `schedule`, `knapsack`, and `cleanup` commands currently exit with
status 1. The three oracle test programs fail for the same reason. Real results will be
recorded in `docs/TestResults.md` once the solvers are merged.

## What pioneer is

pioneer is a command-line program for three planning problems that Pioneer Delivery needs:
choosing non-overlapping deliveries for one driver, choosing deliveries under a fuel budget,
and removing unneeded event-log entries while keeping a required event sequence. Each problem
is solved with dynamic programming.

## Quick start

Toolchain seen in this environment: GCC 14.2.0 and CMake 3.31.6. These are the canonical
build tools for this project.

### 1. Build (verified in this session)

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Both commands exited 0 with no compiler warnings. The build without
`-DCMAKE_BUILD_TYPE=Release` is `PENDING (not yet verified)`.

### 2. Run the tests (verified in this session)

```bash
ctest --test-dir build --output-on-failure
```

Current result: exit status 8. Three of six tests pass: `parser_tests`, `formatter_tests`, and
`cli_checks`. The three oracle tests fail because the solvers are stubs. Each oracle program
first checks its oracle against the expected values (8, 8, and 10 checks passed). Its second
phase then fails with `not implemented`. A clean run is `PENDING` until the solvers are merged.

### 3. Get help (verified in this session)

```bash
build/pioneer --help
```

Exit status 0. The subcommand help (`build/pioneer schedule --help` and the others) is
`PENDING (not yet verified)`.

### 4. Run each command on the example files

```bash
build/pioneer schedule --input examples/schedule.txt
build/pioneer knapsack --input examples/knapsack.txt
build/pioneer cleanup --input examples/cleanup.txt
build/pioneer knapsack --input examples/knapsack.txt --machine
```

In this session each command exited 1 with `pioneer: <Task>: not implemented` on stderr,
because the solvers are stubs. The expected readable output is `PENDING (not yet verified)`.
Reading from standard input (`build/pioneer schedule < examples/schedule.txt`) is
`PENDING (not yet verified)`.

### Options

| Option | Meaning |
|---|---|
| `--input PATH` | Read the problem from PATH. Without it, the problem is read from stdin. |
| `--machine` | Print stable `key=value` lines instead of the readable report. |
| `-h`, `--help` | Show help and exit. |

Exit status: `0` success or help, `1` operational failure (for example, an unreadable file),
`2` usage error or invalid input. Diagnostics go to stderr.

## The three tasks

**Task A: maximum-profit delivery scheduling.** Choose deliveries for one driver so that the
total profit is largest and no two chosen deliveries overlap; a delivery that finishes when
another starts is compatible. The design sorts by finish time, finds each predecessor by binary
search, and runs a weighted interval scheduling DP in O(n log n) time.

**Task B: delivery selection with a fuel budget.** Choose each delivery at most once so that
the total fuel stays within the budget and the total profit is largest. The design is a
bottom-up 0/1 knapsack with a two-dimensional table, which takes O(nB) time and space, so the
running time grows with the numeric value of the budget.

**Task C: event-log cleanup.** Delete as many events as possible from a source string, using
only listed original positions, while a required pattern stays a subsequence of the remaining
string. The design fills a two-dimensional table over source and pattern prefixes in O(nm)
time, then traces back one optimal set of deleted indices.

## Input formats

Summarised from `docs/05_CLI_CONTRACT.md`. Fields are whitespace-separated and blank lines are
ignored. Diagnostics name the failing record or field.

**schedule**

```text
n
id start finish profit
... n records ...
```

- `id`: starts with a letter, then letters, digits, `_`, or `-`. IDs are unique.
- `start`, `finish`: signed 64-bit integers, with `start < finish`.
- `profit`: nonnegative decimal with at most two fractional digits (`50`, `50.5`, `0.01`).
- An empty set is the single line `0`.

**knapsack**

```text
n B
id fuelCost profit
... n records ...
```

- `n`, `B`: nonnegative integers (number of deliveries, fuel budget).
- `fuelCost`: positive integer. `profit`: nonnegative integer.

**cleanup**

```text
source
pattern
k
index0 index1 ... index(k-1)
```

- `source` and `pattern`: nonempty lowercase words on their own lines. `pattern` must already be a
  subsequence of `source`.
- `k`: nonnegative count. The index line is absent when `k` is `0`.
- Indices: strictly increasing, within `[0, |source|)`, and allowed to span lines.

Example for `--machine` output (contract; `PENDING (not yet verified)` against the implementation):

```text
max_profit=180.00
selected_count=3
selected_ids=B,D,F
```

## Tests

Three oracle programs live in `tests/oracles/`. Each one has two phases:

- `ORACLE SELF-CHECKS` runs a brute-force oracle on the named cases from
  `docs/11_TEST_PLAN.md` and compares it with the expected values. The oracle enumerates subsets
  (2^n for scheduling and knapsack, 2^k over eligible indices for cleanup).
- `PRODUCTION COMPARISON` calls the real solver on the same named cases and on seeds 1 to 25,
  compares the optimum with the oracle, and checks the reconstruction invariants.

Named cases covered: A-01 to A-08, B-01 to B-08, and C-01 to C-10 (C-01 to C-04 are the four
official examples). The `parser_tests`, `formatter_tests`, and `cli_checks` programs cover
parsing, formatting, and CLI behaviour.

## PENDING

- **Tested prebuilt Linux download and timing:** PENDING (not yet verified).
- **Final test results:** PENDING. See `docs/TestResults.md`; every actual value is still
  `PENDING`.
- **Attribution and generative AI disclosure:** PENDING (owner-confirmed text).
- **Algorithm report:** `docs/AlgorithmReport.tex` is a skeleton. No TeX installation is
  available in this container, so it has not been compiled. A compiled PDF is PENDING.
