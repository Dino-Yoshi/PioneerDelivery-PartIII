# Pioneer Delivery: dynamic programming tools

`pioneer` is a command-line program for three delivery-planning problems. Each problem is
solved with dynamic programming:

- **schedule**: choose non-overlapping deliveries for one driver with the largest total profit
  (weighted interval scheduling).
- **knapsack**: choose deliveries within a fuel budget with the largest total profit (0/1 knapsack).
- **cleanup**: delete as many events as possible from an event log, using only listed
  positions, while a required event sequence stays a subsequence of the log.

## Quick start

### Toolchain

Any C++20 compiler with CMake 3.20 or newer. No external libraries are needed. These toolchains
built the program with 0 warnings and passed all 9 tests (a continuous-integration run on GitHub Actions, run 43142383981, plus local builds;
see `docs/gates/Portability.md`):

| Platform | Compiler | CMake |
|---|---|---|
| Linux x86_64 (Debian 13 container) | GCC 14.2.0 | 3.31.6 |
| Linux x86_64 (host) | GCC 16.2.1 | 4.4.5 |
| Linux x86_64 (GitHub Ubuntu runner) | GCC 15.2.0 | 4.4.5 |
| macOS (Darwin 25.6, Apple Silicon, GitHub runner) | Apple Clang 21.0.0 | 4.4.5 |
| Windows (GitHub runner) | Microsoft Visual C++ (MSVC) 19.51 | 4.4.5 |

Other compilers and platforms (for example MinGW on Windows) are expected to work but have not
been tested.

### Build

**Linux and macOS** (any shell):

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
```

**Windows** (Visual Studio 2022 or newer, or its Build Tools, plus CMake; run these in PowerShell,
cmd, or a Developer prompt):

```bat
cmake -S . -B build
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

With the Visual Studio generator the program is `build\Release\pioneer.exe`. With a
single-configuration generator (Makefiles, Ninja) on any system it is `build/pioneer`
(`build\pioneer.exe` on Windows). Ninja is optional; add `-G Ninja` if you have it.

### Run one example per task

The commands below use the fixtures in `tests/fixtures/` and the binary path `build/pioneer`. On
Windows with Visual Studio use `build\\Release\\pioneer.exe` and backslashes in paths, for example
`build\\Release\\pioneer.exe schedule --input tests\\fixtures\\A-01.txt`.

```bash
build/pioneer schedule --input tests/fixtures/A-01.txt
```

```text
Weighted interval scheduling
Maximum profit: 180.00
Selected deliveries (3): B, D, F
```

```bash
build/pioneer knapsack --input tests/fixtures/B-01.txt
```

```text
Delivery selection with a fuel budget
Maximum profit: 53
Total fuel used: 10
Selected deliveries (3): Q, R, S
```

```bash
build/pioneer cleanup --input tests/fixtures/C-01.txt
```

```text
Event log cleanup
Maximum deletions: 1
Removed original indices (1): 2
```

### Machine-readable output

Add `--machine` to print stable `key=value` lines:

```bash
build/pioneer schedule --input tests/fixtures/A-01.txt --machine
```

```text
max_profit=180.00
selected_count=3
selected_ids=B,D,F
```

The knapsack keys are `max_profit`, `total_fuel`, `selected_count`, and `selected_ids`. The
cleanup keys are `max_deletions`, `removed_count`, and `removed_indices`.

### Run the tests

```bash
ctest --test-dir build --output-on-failure
```

All 9 tests pass: `SchedulingTests`, `KnapsackTests`, `CleanupTests`, three oracle programs
(`SchedulingOracleTests`, `KnapsackOracleTests`, `CleanupOracleTests`), `parser_tests`,
`formatter_tests`, and `cli_checks`. The full results are in [docs/TestResults.md](docs/TestResults.md).

### Get help

```bash
build/pioneer --help
build/pioneer knapsack --help
```

Each subcommand help prints its input grammar and a small example.

## Commands and options

| Option | Meaning |
|---|---|
| `--input PATH` | Read the problem from `PATH`. Without it, the problem is read from standard input. |
| `--machine` | Print `key=value` lines instead of the readable report. |
| `-h`, `--help` | Show help and exit. |

Exit status:

| Code | Meaning |
|---:|---|
| 0 | Success or help. |
| 1 | Operational failure, such as an unreadable file or a DP table over the size limit. |
| 2 | Usage error or invalid input. |

Diagnostics go to standard error. Nothing is printed to standard output after a failure.

## Input format

Fields are whitespace-separated. Blank lines are ignored.

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

- `n`, `B`: nonnegative integers. `n` is the number of deliveries and `B` is the fuel budget.
- `fuelCost`: positive integer. `profit`: nonnegative integer.

**cleanup**

```text
source
pattern
k
index0 index1 ... index(k-1)
```

- `source` and `pattern`: nonempty lowercase words, each on its own line. `pattern` must already
  be a subsequence of `source`.
- `k`: nonnegative count. When `k` is `0`, there is no index line.
- Indices: strictly increasing, inside `[0, length of source)`. They may span several lines.

## Standard input

Without `--input`, a command reads standard input. This works the same on every platform, and
the output matches `--input`:

```bash
build/pioneer schedule < examples/schedule.txt          # Linux, macOS, cmd
```

```powershell
Get-Content examples\schedule.txt | build\Release\pioneer.exe schedule    # PowerShell
```

PowerShell has no `<` redirection, so pipe the file in as shown. All of these were run in
continuous integration on Windows (PowerShell and cmd), macOS and Linux, and for all three
commands on Linux (orchestrator check, `docs/gates/G3.md`).

## Input files from Windows

Input files may use LF or CRLF line endings, and may start with a UTF-8 byte order mark (which
Notepad can add); the mark is ignored. A UTF-16 file is rejected with a clear message (exit 2):
Windows PowerShell 5.1 writes UTF-16 with `>` and `Out-File` by default (PowerShell 7 writes UTF-8), so save inputs as UTF-8 or ASCII,
for example with `Out-File -Encoding ascii`. These cases are covered by the `cli_checks` test.

## Size limits

The knapsack and cleanup solvers allocate a two-dimensional table. The table is limited to
256 MiB (268,435,456 bytes) and is refused before allocation when it would be larger.

- Knapsack: `(n+1) x (B+1)` cells of 8 bytes. The limit is 33,554,432 cells.
- Cleanup: `(n+1) x (m+1)` cells of 4 bytes, where `n` is the source length and `m` is the pattern
  length. The limit is 67,108,864 cells.

Knapsack with one delivery and budget `999999999999` fails with exit status 1. The run used
`tests/fixtures/limit-knapsack-table.txt`:

```text
pioneer: knapsack table: (n+1)=2 by (B+1)=1000000000000 is 2000000000000 cells, which exceeds the limit of 268435456 bytes (33554432 cells)
```

## The three tasks

**Task A: schedule.** The solver sorts deliveries by finish time and finds each delivery's
predecessor by binary search. The DP then runs in O(n log n) time with O(n) auxiliary space.

**Task B: knapsack.** A bottom-up 0/1 knapsack fills a table of `(n+1) x (B+1)` cells. The time
and space are O(nB), so the running time depends on the value of the budget, not on how many
digits it has.

**Task C: cleanup.** The solver fills a table over prefixes of `source` and `pattern`, and then
traces back one optimal set of deleted indices. The time is O(nm) and the auxiliary space is
O(nm).

## Test evidence and report

- [docs/TestResults.md](docs/TestResults.md): one row per named case with input summary, expected
  value, actual output, exit code, and check result.
- [docs/AlgorithmReport.tex](docs/AlgorithmReport.tex): the algorithm report source. It is built
  to `docs/AlgorithmReport.pdf` with `latexmk -pdf` (needs a TeX installation with the packages
  booktabs, listings, hyperref and geometry).
- `tests/fixtures/`: the named test inputs (`A-01` to `A-08`, `B-01` to `B-08`, `C-01` to `C-10`)
  and invalid inputs (`invalid-*.txt`).

## Repository layout

```text
CMakeLists.txt             build and test definitions
include/pioneer/           public headers: Input, Output, Scheduling, Knapsack, Cleanup
src/                       parsers, formatter, the three solvers, and main.cpp (command line)
tests/                     unit tests, CLI checks, and the test harness
tests/oracles/             independent brute-force oracle programs
tests/fixtures/            named and invalid input files
examples/                  one example input per command
docs/                      CLI contract, test plan, TestResults.md, AlgorithmReport.tex, gate records
```

## Prebuilt Linux quick start

A statically linked x86_64 Linux archive can be built from this repository with
`bash tools/package-linux.sh` (needs `cmake`, `ninja`, `strip`, `readelf`, `tar`, `sha256sum`;
it writes `dist/pioneer-linux-x86_64.tar.gz` and a `.sha256` file; two builds of the same sources
produced byte-identical archives). The archive holds the `pioneer` binary, a one-page
`README.txt`, and the three example inputs. Nothing else is needed to run it.

Tested: the archive with SHA-256 `932510957cea3db619c56f769c6b8ffc039438b9ae681650e55c6bdc3548f802` was extracted and run in a clean
Debian GNU/Linux 13 (trixie) x86_64 container that has no compiler, no CMake and no network, and all
three examples printed the expected results. Other distributions and CPU types are not tested.
The archive is not published anywhere; build it with the script above.

## Development and attribution

- Original Pioneer Delivery developer: the project owner, who is also the Part III owner.
- Human contribution: the project owner set up the project and tools, planned the work, made
  the executive decisions, made the check-ins on the implementation, managed the final
  deliverables, and handled the tasks the agents could not do themselves.
- AI assistance: the design handoff documents (specifications, tickets and worker briefs) were
  prepared with GPT-6 (High reasoning setting), as reported by the project owner.
- AI assistance: Claude Code with Claude Sonnet 5.5 acted as orchestrator. It read the
  assignment and the design handoff, created the repository and branches, dispatched and reviewed
  the workers, re-ran every build and test itself, ran a mutation test of the test suites, merged
  the branches, wrote the gate records, and made the Git commits.
- AI assistance: Kiro CLI with Claude Haiku 5.5 ran five worker sessions. W1 built the CMake/CTest
  skeleton, input parsers, output formatter, command-line interface, and help. W2 implemented Task A
  (scheduling). W3 implemented Task B (knapsack). W4 implemented Task C (cleanup). W5 wrote the
  independent brute-force oracle programs, test fixtures, and documentation drafts.
- Verification: independent clean builds in a container (GCC 14.2, CMake 3.31) and on the host
  (GCC 16.2, CMake 4.4); 9 of 9 CTest tests; brute-force oracle comparison on named cases plus
  seeds 1 to 25; a four-bug mutation test; end-to-end CLI checks. The numbers come from
  `docs/gates/`.
- Lesson learned: Sequence alignment, which is Task C, was the hardest part for me, and its mostly because I didn't know where to start. At first, my plan was to loop over the strings and delete what seemed appropriate. Most of my progress came from reading, speculation, and guesswork, not from solving it on my own. The tests passed, but I don't consider that the same as understanding it. Truthfully, I do a lot better on things when I have existing practice surrounding those issues beforehand. To do something that I don't feel confident in, my approach will most definitely show it.
