# RELEASE-01 — Integrated release, source ZIP and human signoff

- **Status:** NOT STARTED
- **Owner:** Orchestrator
- **Prerequisite:** all prior tickets + G5/G5-U
- **Acceptance gate:** Release gate
- **Design authority:** `../00_REQUIREMENTS_TRACEABILITY.md` and applicable detailed spec, plus original professor PDF.

## Scope

Perform final clean integration, full regression verification, documentation/attribution audit, source ZIP packaging and release promotion only with human approval.

## Acceptance criteria

1. All gates PASS with evidence; no unresolved critical defects or unsigned documentation claims.
2. Verified integration merged/tagged on main only after explicit owner approval.
3. Source ZIP contains single top-level project folder, buildable source/test fixtures, README, actual Markdown test results, .tex and PDF report.
4. ZIP extracted in clean directory and rebuilt/tested using README commands, with correct output for all three examples.
5. No build dirs, secrets, operational session state or unapproved generated binaries in Canvas source ZIP.
6. Archive hash/manifest and accurate report to user recorded; Canvas upload itself requires separate explicit permission.

## Evidence required

- integration/main commit IDs
- fresh build and CTest logs
- ZIP archive checksum/manifest
- release approval record

## Out of scope

No unilateral pushes, external publication, Canvas submission, or destructive branch cleanup.

## Handoff / completion contract

Report changed paths, current branch/worktree, actual commands and exit statuses, concrete failures, tests/fixtures, blocker decisions, and session/checkpoint reference. No worker may commit/merge/push; orchestrator reviews all changes before acceptance.
