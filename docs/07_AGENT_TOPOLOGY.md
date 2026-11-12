# 07 — Orchestrator, Workers, Tickets, Branches, and Concurrency

## Hierarchy

```text
Human project owner (requirements, decisions, final approval)
  |
  +-- Claude Code — Sonnet 5.5 / Medium (SOLE orchestrator and human-facing facade)
       |
       +-- W1 Kiro CLI / Haiku 5.5 Medium — Foundation + CLI + operational wrapper
       +-- W2 Kiro CLI / Haiku 5.5 Medium — Weighted scheduling (A)
       +-- W3 Kiro CLI / Haiku 5.5 Medium — 0/1 knapsack (B)
       +-- W4 Kiro CLI / Haiku 5.5 Medium — Subsequence cleanup (C)
       +-- W5 Kiro CLI / Haiku 5.5 Medium — Independent QA + Task D docs
       |
       +-- Integration: reviewed feature branches -> integration -> main
```

The orchestrator is **not** another peer feature worker. It interprets tickets, starts/stops Kiro CLI processes, captures artifacts, reviews code, commits, merges, runs gates, reports to the owner, and enforces release approval. Workers do not communicate assumptions directly to each other: interface changes are requests routed to the orchestrator.

## Worker ownership

| Worker | Tickets | Owned files/areas | Forbidden scope |
|---|---|---|---|
| W1 Foundation | SETUP-01, CORE-01, OPS-01 | `CMakeLists.txt`, CLI/parser/output, shared headers, dev tooling | Production algorithm recurrences |
| W2 Scheduling | A-01, A-02 | `Scheduling.hpp/.cpp`, `SchedulingTests.cpp` | Other algorithms, top-level CLI/build |
| W3 Knapsack | B-01, B-02 | `Knapsack.hpp/.cpp`, `KnapsackTests.cpp` | Other algorithms, top-level CLI/build |
| W4 Cleanup | C-01, C-02 | `Cleanup.hpp/.cpp`, `CleanupTests.cpp` | Other algorithms, top-level CLI/build |
| W5 QA / Docs | TEST-01, DOC-01, DOC-02, UX-01 (test side) | `tests/oracles/`, `README.md`, `docs/AlgorithmReport.tex`, `docs/TestResults.md`, usability evidence | Editing production algorithms to make tests pass |
| Orchestrator | RELEASE-01, integration, audit | integration/main worktrees, all gate records, final release | Silent rewriting of worker code/unauthorized scope |

Cross-owned modifications require an orchestrator-issued follow-up ticket. W1 may wire tests into CMake at integration without changing their assertions. W5 can propose CLI usability fixes; W1 implements them. W4 has heightened review scrutiny due to nuanced DP traceback.

## Ticket dependency graph

```text
G0 environment smoke test
         |
   SETUP-01 ---> OPS-01
         |
      CORE-01 ---> G1 interface contract verified
         |                \
         +--> A-01 --> A-02 \
         +--> B-01 --> B-02  +--> TEST-01/G3 --> G4
         +--> C-01 --> C-02 /
         +--> DOC-01 ------/ 
                           DOC-02 + UX-01 -> G5/G5-U -> RELEASE-01
```

W5 can draft fixtures and report/README skeleton while algorithm work proceeds. Independent oracle development must not inspect production DP implementation logic first; read the professor specification and algorithm problem definition instead.

## Git branches / worktrees

- `main`: only verified, approved release-ready state.
- `integration`: orchestrator-owned merge/test staging branch.
- `feature/foundation`, `feature/scheduling`, `feature/knapsack`, `feature/cleanup`, `feature/verification-docs`: worker-specific development branches.
- One separate worktree per worker, plus orchestrator-controlled integration checkout and stable `main` checkout.
- Feature branches for W2–W5 must start from the **post-G1 integrated foundation commit**, not an earlier empty repository.

Illustrative layout (root path is configurable, not hardcoded):

```text
<project-root>/
  main/               [main]
  integration/        [integration]
  worktrees/
    foundation/       [feature/foundation]
    scheduling/       [feature/scheduling]
    knapsack/         [feature/knapsack]
    cleanup/          [feature/cleanup]
    verification/     [feature/verification-docs]
```

A linked Git worktree contains a `.git` pointer to shared metadata. Worker containers must **not** receive writable access to that common metadata; see `08_ISOLATION_AND_SECURITY.md`. Orchestrator commits from host-managed Git worktrees after inspecting changes and passing checks. If containers use a staged copy rather than a direct worktree mount, patches/diffs must be applied only into the designated branch by the orchestrator, with provenance retained.

## Concurrency scheduling

1. G0 preflight, safe tool/model/auth check, non-destructive smoke test.
2. W1 creates foundation; orchestrator reviews and integrates it; freeze interfaces at G1.
3. Start W2/W3/W4 in parallel; W5 starts independent oracle fixtures and documentation skeleton. W1 may work on OPS-01 and CLI polish if no file collisions.
4. Workers produce checkpointed, scoped outputs; orchestrator accepts and commits sequentially.
5. Merge one verified branch/ticket at a time, rerunning relevant checks. G3 may reveal defects requiring return tickets.
6. Full G4 test on integration; G5 documentation/release audit; G5-U usability trial; owner approval; promote `main`/archive.

## Escalation

W4 and any worker with repeated failures should receive a **specific failing test + invariant**, not vague 'try again'. If a targeted Haiku repair still fails, Sonnet diagnoses and may authorize a narrower task or direct correction, recording it accurately. Do not silently swap workers/models to create apparent success. Limit auto-retries to a configured small count and report genuine blocks.
