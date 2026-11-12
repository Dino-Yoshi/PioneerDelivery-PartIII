# TEST-01 — Independent exhaustive oracles and integration verification

- **Status:** ACCEPTED at G3 (oracle programs, integration); final evidence recording continues in DOC-02
- **Owner:** W5
- **Prerequisite:** CORE-01; algorithm results for final runs
- **Acceptance gate:** G3
- **Design authority:** `../00_REQUIREMENTS_TRACEABILITY.md` and applicable detailed spec, plus original professor PDF.

## Scope

Build specification-derived reference oracles and deterministically seeded small-instance property tests independently of production algorithm implementations.

## Acceptance criteria

1. A subsets enumerated with direct interval compatibility, B subsets enumerated with direct fuel/profit sums, C eligible deletion subsets enumerated with independent subsequence check.
2. Oracles do not call production DP recurrence/predecessor/traceback logic.
3. At least one small independent optimum for both A and B as professor requires; C oracle recommended.
4. Run named cases A>=4, B>=4, C>=4 (all four PDF examples) and extra adversarial cases; verify reconstruction invariants.
5. Fixed seeds and failed instances logged; no hardcoded single optimum IDs required.
6. Tests registered with CTest and reproducible in clean environment.

## Evidence required

- oracle review methodology
- fixed seed list and case traces
- CTest stdout/exit code
- real expected vs actual summary

## Out of scope

Never edit algorithms to make oracle results agree; report discrepancies to orchestrator.

## Handoff / completion contract

Report changed paths, current branch/worktree, actual commands and exit statuses, concrete failures, tests/fixtures, blocker decisions, and session/checkpoint reference. No worker may commit/merge/push; orchestrator reviews all changes before acceptance.
