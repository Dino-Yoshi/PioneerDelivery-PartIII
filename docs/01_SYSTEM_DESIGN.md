# 01 — System Architecture and Product Boundary

## Goal and constraints

Deliver one small, understandable C++20 command-line application solving three distinct dynamic-programming tasks, with traceable correctness, simple installation, discoverable usage, independently reproducible tests, and all professor-required Task D materials.

**Out of scope:** GUI, network service, database, cloud runtime for end users, external AI dependency for runtime, additional algorithms as production fallbacks, advanced optimization that violates the assigned DP structure.

## Application architecture

```text
      user / input file / stdin
                |
      CLI option + format parser
                |
        validated input structs
           /      |      \
   Scheduling  Knapsack   Cleanup       <-- pure algorithm modules
           \      |      /
         structured result
                |
    human presentation / --machine
                |
         stdout / stderr
```

Algorithm modules know nothing about arguments, input streams, terminal formatting, or Git/agents. The CLI should not contain DP logic. Input validation is explicit and consistent; algorithms may defend against direct callers where inexpensive. Core functions receive validated records and return result data.

## Core module responsibilities

| Layer | Owns | Does not own |
|---|---|---|
| `main.cpp` | Dispatch, options, --help, exit status | DP tables |
| `Input.cpp` (+ header when useful) | Task-specific parsing, numeric conversion, validation | Optimization |
| `Scheduling.*` | Sorting by finish, predecessors, weighted DP, traceback | CLI |
| `Knapsack.*` | Two-dimensional budget DP, traceback | CLI |
| `Cleanup.*` | Source/pattern DP, deletion traceback | CLI |
| Output formatter | Human/default and machine modes | Recomputing optimum |
| `tests/` | Unit, CLI, oracle/property, reconstruction checks | Production decisions |

## Proposed public data contract (names illustrative)

- `DeliveryInterval`: `id` string, `start`/`finish` int64, `profitCents` int64.
- `SchedulingResult`: `maximumProfitCents` int64, selected ID sequence.
- `FuelDelivery`: `id` string, `fuelCost` positive integer, `profit` nonnegative int64.
- `KnapsackResult`: `maximumProfit`, `totalFuel`, selected ID sequence.
- `CleanupInput`: lowercase `source`, lowercase `pattern`, ascending original target positions.
- `CleanupResult`: `maximumDeletions`, ascending original indices.

Use standard C++ containers and checked arithmetic. The real header signatures are established by W1 at G1, then considered frozen until orchestrator-approved modification.

## Design philosophy

1. **Human-readable code first** while preserving required asymptotic performance.
2. Prefer small named functions over opaque compound expressions; keep control flow obvious.
3. Use types and constraints to make invalid states difficult; do not create needless class hierarchies.
4. Do not require an algorithm caller to understand the CLI input grammar.
5. Keep test-only exhaustive references in test modules, separate from production code.
6. Ensure deterministic tie policies while **testing for optimum and validity**, not a particular optimal set.

## Execution boundary

The end-user binary is independent of the AI execution topology. Claude Code, Kiro, Podman/Docker, `agentctl`, and Git worktrees are development tools, **not** runtime or grading dependencies. The Canvas ZIP must be buildable using documented C++/CMake dependencies and contain all required sources, fixtures, and reports.

## Definition of finished product

A fresh checkout/source ZIP cleanly configures/builds with CMake, passes CTest, runs each of the three professor examples and additional cases, emits correct reconstructed outputs, includes actual recorded tests and LaTeX-compiled PDF, and passes a timed cold-start usability trial on a **tested supported Linux prebuilt package**. Do not make broader platform claims without evidence.
