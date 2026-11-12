# 05 — CLI and Plain-Text Input/Output Contract

**Contract status:** approved design; establish actual parser/formatter APIs at G1. CLI examples below are **desired behavior**, not commands verified against an implemented binary.

## Commands

```text
pioneer --help
pioneer schedule [--input PATH] [--machine]
pioneer knapsack [--input PATH] [--machine]
pioneer cleanup  [--input PATH] [--machine]
pioneer <subcommand> --help
```

No arguments displays help and exits successfully; an unknown command/option yields usage plus nonzero error. With `--input PATH`, read that file; otherwise read stdin. Support `pioneer schedule < file.txt`. `--machine` means stable plain UTF-8 `key=value` lines, fixed keys/order, no extra noise on stdout. Human-readable, clearly labeled results are the default.

The application requires no AI/cloud/runtime account. Return diagnostics on **stderr**, not stdout. Exit `0` on success/help, `2` on usage/validation failure, and `1` on operational failures (e.g. file read or allocation limits); document any refinement before tests are finalized. Never print partial success output after failure.

## Shared parser rules

- UTF-8 or ASCII text using whitespace as delimiters; IDs are ASCII `[A-Za-z][A-Za-z0-9_-]*`, no whitespace, and unique per instance. No quoted IDs.
- Integers are explicitly decimal, parsed with bounds checking; no locale-dependent parsing. Never silently truncate fractional or oversized input.
- Except for line-specific source/pattern fields, blank lines may be ignored; comments are **not** part of the input grammar unless separately agreed.
- Accept required whitespace-separated fields and reject extra non-whitespace tokens after the expected record count.
- Validate constraints and explain the offending field/record in errors. On invalid input, do not modify the input file.
- Large inputs that exceed safe DP memory/resource limits produce explicit errors; never fall back to an algorithm that violates the PDF.
- Numeric output is locale-independent; Task A prints `D.CC` with exactly two digits after decimal.

## `schedule` input

```text
n
id start finish profit
... n records ...
```

`n` nonnegative integer. `start`, `finish` signed 64-bit integers with `start<finish`. `profit` nonnegative decimal text, at most two fractional digits (e.g. `50`, `50.0`, `50.00`, `0.01`), exactly parsed as int64 cents. Empty set is exactly `0` followed by optional whitespace. Duplicate IDs and overflow rejected.

Official example:

```text
6
A 0 3 50.00
B 1 4 60.00
C 3 5 40.00
D 4 7 70.00
E 5 8 50.00
F 7 9 50.00
```

`--machine` **exact keys**:

```text
max_profit=180.00
selected_count=3
selected_ids=B,D,F
```

For empty selection: `selected_count=0`, `selected_ids=`. IDs are comma-separated without spaces, chronological sorted-by-finish order. If two optima tie, selected IDs may differ; tests must validate values/invariants rather than string equality of IDs.

## `knapsack` input

```text
n B
id fuelCost profit
... n records ...
```

`n` and budget `B` are nonnegative decimal integers; each fuel cost positive; each profit nonnegative integer, all range-checked. Example:

```text
5 10
P 4 20
Q 3 14
R 5 29
S 2 10
T 6 31
```

`--machine` **exact keys**:

```text
max_profit=53
total_fuel=10
selected_count=3
selected_ids=Q,R,S
```

IDs in original **input order**; `total_fuel` computed from selected records, not simply set to budget. Empty selection uses `0`, `0`, `0`, and blank ID value. `max_profit` must equal sum of selected profits.

## `cleanup` input

```text
source
pattern
k
index0 index1 ... index(k-1)
```

First two **nonblank** lines each contain a single nonempty lowercase ASCII word. Next is nonnegative count k; subsequent k indices are whitespace-separated and may span lines. For k=0 no index line is required. `pattern` must initially be a subsequence of `source`. All indices must be in `[0,|source|)`, strictly increasing and unique. Reject non-lowercase letters, invalid positions, and extra tokens.

Official example:

```text
abbaa
aba
3
0 1 2
```

One valid `--machine` output:

```text
max_deletions=1
removed_count=1
removed_indices=2
```

`removed_indices` always ascending original zero-based indices separated by commas; blank when none. Another optimum may use index 1. Match oracle count and deletion validity, not a single fixed index set.

## Human-readable presentation

Keep consistent, short descriptive labels. Suggested:

```text
Weighted interval scheduling
Maximum profit: 180.00
Selected deliveries (3): B, D, F
```

For the other tasks show `Maximum profit`, `Total fuel used`, selected IDs, or `Maximum deletions`, removed original indices. A missing selection must say `None` (not appear broken). Do not print giant DP tables in normal results; those belong in docs/debugging, not output.

## Discoverability and errors

`--help` should give the purpose, grammar, one copy-paste example, `--input`/stdin behavior, `--machine`, and exit codes; task help should include one compact sample input. Validation examples: `schedule: record 3: finish must be greater than start`; `cleanup: target index 7 is out of bounds for source length 5`. No stack traces for ordinary user errors.

## CLI integration tests

Verify file/stdin parity, --help root/subcommands, machine key order, blank lists, invalid flag/input/extra tokens, stderr+exit status, and that every published command actually works with release package layout. This is a G4/G5-U release condition.
