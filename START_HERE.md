# Pioneer Delivery Part III — Orchestrator Handoff

This is the **design handoff**, not a finished application. It packages the source assignment, frozen architecture, algorithm contracts, worker briefs, tickets, security/gate policy, operational recovery specification, and Task D deliverables for a **Claude Code / Sonnet 5.5 Medium** orchestrator directing **Kiro CLI / Haiku 5.5 Medium** workers.

## Getting started

1. Place this folder's contents at the **root of the new, independent Part III repository** (or run Claude Code from this folder as the future repository root). The root filename must be exactly `CLAUDE.md`.
2. Open Claude Code with Sonnet 5.5 / Medium, after verifying that model/effort are available in your account.
3. Claude Code normally loads root `CLAUDE.md` automatically at session start. To be explicit, give the following instruction:

> Read `CLAUDE.md` and all referenced authoritative specifications. First summarize the frozen architecture, tickets, G0 preflight checks, and execution order. Do not implement anything until I authorize execution.

4. Once you approve the plan, ask Sonnet to perform G0, then G1, then the parallel-worker sequence. Follow the quality gates and record evidence.

## What is already decided

- Independent Git repository, one CLI executable `pioneer` with three subcommands.
- C++20, CMake, CTest, separate readable algorithm modules, no needless libraries.
- Weighted scheduling, 0/1 knapsack, and source/pattern deletion DP with mandated complexities, reconstruction, and independent tests.
- Separate Haiku workers under one Sonnet orchestrator; rootless worker containers and isolated worktrees; orchestrator-controlled Git integration.
- Plain-text input; human output + `--machine`; short README and LaTeX PDF report; five-minute tested quick-start goal.
- Actual results, report's learned lesson, and attribution details must reflect the work that really happens.

## Deliverable inventory

- `CLAUDE.md`: compact mandatory orchestration rules.
- `reference/PA-PartIII-2026Fall.pdf`: the original assignment (SHA-256 `56d5510955501d1d222642adbe54848887c13bf7f4c39505e69b58c35e1c4731`).
- `docs/00_...` through `docs/18_...`: detailed specifications and runbooks.
- `docs/tickets/`: ticket-level scope, ownership, dependencies and acceptance evidence.
- `docs/workers/`: per-worker bounded briefs.
- `docs/templates/`: handoff/gate/test/attribution/lesson/report templates.

## No false completion

There is **no C++ implementation, configured build, test execution, compiled PDF report, container image, running orchestration service, `agentctl` executable, or release ZIP** in this handoff. The future orchestrator creates/verifies these during authorized implementation.

## Decision precedence

1. Actual professor PDF for grading requirements.
2. Explicit human choices recorded in the design documents for optional implementation details.
3. Further documented defaults/proposals in this handoff, subject to G0 and human review when material.

Contact the owner for changes to the frozen design, unavailable authentication/model/tooling, non-trivial risks, or release authorization. Do not silently bypass safeguards.
