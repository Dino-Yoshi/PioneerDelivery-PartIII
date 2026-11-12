# OPS-01 — Isolated worker lifecycle and resume utility

- **Status:** SKIPPED by owner decision (2026-11-11); see docs/17
- **Owner:** W1
- **Prerequisite:** SETUP-01
- **Acceptance gate:** G1 plus operational proof
- **Design authority:** `../00_REQUIREMENTS_TRACEABILITY.md` and applicable detailed spec, plus original professor PDF.

## Scope

Create minimal internal agentctl or equivalent wrapper for orchestrator-managed isolated Kiro workers; implement actual resume based on exact verified session IDs. Do not add operational tooling to product runtime.

## Acceptance criteria

1. Commands start/status/logs/checkpoint/resume/retry/stop/inspect have documented usable behavior or explicitly scoped aliases.
2. One worker lock prevents simultaneous worktree/session access.
3. State schema survives restart, uses atomic updates, stores real model ID/session ID/container ID and redacted logs.
4. Container mounts enforce exclusive workspace, persistent session home, no writable common Git metadata or host secrets.
5. Resume uses exact --resume-id in the same logical workspace; retry is bounded and never silently restarts from a different state.
6. Failure paths fail closed; secrets do not appear in logs/Git.

## Evidence required

- container capability denial tests
- start/stop/resume smoke log
- lock/corrupt-state tests
- actual session continuity evidence

## Out of scope

No general-purpose agent platform, arbitrary remote access, or privilege escalation.

## Handoff / completion contract

Report changed paths, current branch/worktree, actual commands and exit statuses, concrete failures, tests/fixtures, blocker decisions, and session/checkpoint reference. No worker may commit/merge/push; orchestrator reviews all changes before acceptance.
