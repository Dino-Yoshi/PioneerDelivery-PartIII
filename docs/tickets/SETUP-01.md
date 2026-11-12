# SETUP-01 — Repository and build foundation

- **Status:** NOT STARTED
- **Owner:** W1
- **Prerequisite:** none
- **Acceptance gate:** G1
- **Design authority:** `../00_REQUIREMENTS_TRACEABILITY.md` and applicable detailed spec, plus original professor PDF.

## Scope

Initialize the independent repository and its stable main/integration/worktree topology only after the G0 security and tool preflight. Create clean CMake/C++20/CTest foundation and project directory structure without implementing any optimization logic.

## Acceptance criteria

1. Independent Git repository exists with clean initial baseline and planned worktree structure.
2. CMake configures/builds a minimal C++20 program on the tested toolchain; CTest discovers a smoke test.
3. No premature AI/runtime dependencies in the product build.
4. A correct .gitignore excludes build, private operational state, credentials and generated binaries.
5. Foundation passes orchestrator code review and integrates without unauthorized remote pushes.

## Evidence required

- G0 tool+isolation report
- Git branch/worktree listing (no secrets)
- actual CMake/CTest logs
- initial commit hash

## Out of scope

No algorithm recurrences, UI framework, network service, or remote publication.

## Handoff / completion contract

Report changed paths, current branch/worktree, actual commands and exit statuses, concrete failures, tests/fixtures, blocker decisions, and session/checkpoint reference. No worker may commit/merge/push; orchestrator reviews all changes before acceptance.
