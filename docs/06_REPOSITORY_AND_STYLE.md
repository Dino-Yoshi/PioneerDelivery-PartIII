# 06 — Repository, Build, and Human-Centered C++ Style

## Independent Git repository

Part III is **not a feature branch of Parts I/II**. Preserve the visual/documentation identity where verifiable, but do not reuse earlier implementations implicitly. Suggested repository name: `PioneerDelivery-PartIII` (not a requirement imposed by the assignment). Do not assume a remote exists or push without approval.

## Planned product layout

```text
PioneerDelivery-PartIII/
├── CLAUDE.md                    # Orchestrator instructions
├── CMakeLists.txt
├── README.md                    # End-user quick start (created during development)
├── .gitignore
├── include/pioneer/
│   ├── Scheduling.hpp
│   ├── Knapsack.hpp
│   ├── Cleanup.hpp
│   └── Input.hpp               # Only if parser API benefits from public header
├── src/
│   ├── main.cpp
│   ├── Input.cpp
│   ├── Scheduling.cpp
│   ├── Knapsack.cpp
│   └── Cleanup.cpp
├── tests/
│   ├── SchedulingTests.cpp
│   ├── KnapsackTests.cpp
│   ├── CleanupTests.cpp
│   └── oracles/                 # W5 independent small-instance verification
├── examples/
│   ├── schedule.txt
│   ├── knapsack.txt
│   └── cleanup.txt
├── docs/
│   ├── AlgorithmReport.tex     # Author in LaTeX
│   ├── AlgorithmReport.pdf     # Actual compiled submission PDF
│   └── TestResults.md          # Real, executed test outcomes
├── tools/                       # Orchestration/dev tools, not runtime dependencies
├── reference/                   # Assignment source and planning reference
└── ... planning docs and ticket briefs ...
```

The exact internal header name `Input.hpp` may be decided by W1; keep the module/interface contract, not unnecessary files. This handoff currently contains planning docs in `docs/` and `reference/`. The project ZIP may exclude internal operational planning and credentials but must retain all rubric-required deliverables.

## Style rules (user-approved)

- **Types, structs, enums:** `PascalCase`, e.g. `DeliveryInterval`, `SchedulingResult`.
- **Functions, methods, variables, fields:** `camelCase`, e.g. `findPredecessor`, `totalFuel`, `profitCents`.
- **Constants:** descriptive `kCamelCase` or named `constexpr` consistently as approved by W1; avoid undocumented magic numbers.
- Public interfaces are small, ordinary C++20 types; avoid unnecessary inheritance, factories, singleton registries, DSLs, and patterns.
- Prioritize **readability, not code golf**. The shortest implementation is not necessarily the best.
- No macros where named constexpr/functions suffice. Avoid clever metaprogramming and external libraries merely to save a few lines.
- Use explicit signed/unsigned conversion, safe indexing, overflow handling, and clear types for cents/time/budget.
- Use comments for invariants, nonobvious edge cases, recurrence mapping, and why a tie policy is used. Avoid repeating each source statement in English.
- Each algorithm's DP state/base/transition/traceback should be obvious to a competent reviewer reading its module.
- Keep parsing, core logic, and presentation separate. No global mutable state in the algorithm modules.
- Prefer deterministic record sorting and chosen-solution ordering. No accidental dependence on hash iteration order.

## Build and tests

Expected after implementation (not yet verified):

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
ctest --test-dir build --output-on-failure
./build/pioneer --help
```

Provide compiler version and CMake minimum version in the README based on tools actually tested at G0/G4. Use self-contained C++ test executables registered with CTest; avoid external GoogleTest/Catch2 unless approved. `.gitignore` excludes `build/`, derived binaries, temporary logs, operational state, editor files, credentials. **Do not exclude actual LaTeX source or final PDF report.**

## Readability review questions

- Can a new reader identify input assumptions, state, recurrence, and result without hunting across CLI files?
- Are there unneeded layers, multipurpose functions, or code generation that obscure intent?
- Are boundary checks done before allocation and potentially overflowing operations?
- Do comments clarify ideas rather than duplicate obvious statements?
- Is each function testable without invoking CLI or reading files?

## Commit practices

Commits should be scoped to ticket-owned changes, clearly named, and traceable to worker and reviewer in internal gate records. The orchestrator owns actual commits. Do not fabricate human co-author lines; the README describes real human and AI involvement. Never put secrets, tokens, model transcripts, or session databases into Git.
