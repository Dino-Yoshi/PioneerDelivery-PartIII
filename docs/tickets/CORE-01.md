# CORE-01 — Shared interfaces, CLI parsing, output, help

- **Status:** ACCEPTED at G1 (2026-11-11), integration 250c219
- **Owner:** W1
- **Prerequisite:** SETUP-01
- **Acceptance gate:** G1
- **Design authority:** `../00_REQUIREMENTS_TRACEABILITY.md` and applicable detailed spec, plus original professor PDF.

## Scope

Define minimal validated data/result structs, task-specific plain-text parsers, the single pioneer command dispatcher, task help, default human and --machine renderers. Freeze interfaces after orchestrator acceptance.

## Acceptance criteria

1. CLI exposes schedule, knapsack, cleanup and root/subcommand --help.
2. Task A times parse int64 and profits parse exact int64 cents with <=2 fractional digits; Task B positive integer costs and budget; Task C valid lowercase strings and sorted original indices.
3. File and stdin inputs produce equivalent parsed instances; extra tokens, duplicates and invalid constraints fail explicitly.
4. Machine output follows docs/05_CLI_CONTRACT.md exact keys/order and empty selection representation.
5. Parsing/output modules contain no DP logic and can call independently compiled algorithm interfaces.
6. Exit statuses/stderr diagnostics are documented and tested.

## Evidence required

- public-header review
- CLI/help smoke outputs
- parser fixture tests and exits
- interface freeze record

## Out of scope

No algorithm implementations, no extra serialization framework, no silent fallback of invalid inputs.

## Handoff / completion contract

Report changed paths, current branch/worktree, actual commands and exit statuses, concrete failures, tests/fixtures, blocker decisions, and session/checkpoint reference. No worker may commit/merge/push; orchestrator reviews all changes before acceptance.
