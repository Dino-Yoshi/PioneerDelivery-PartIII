# 09 — `agentctl`: Minimal Worker Lifecycle and Resume Specification

`agentctl` is an **internal development utility to be implemented under OPS-01**, not a product dependency. It is a thin process/container/session wrapper, **not another AI agent**. Do not claim it exists yet.

## Goals

Give Claude Code/Sonnet a small, auditable command surface to launch, inspect, pause/stop, and resume Kiro CLI workers without losing their work or accidentally launching two sessions in one worktree.

## Proposed interface (to implement and verify)

```text
agentctl status [W1|W2|W3|W4|W5]
agentctl start W2 --ticket A-01
agentctl logs W2 [--tail N]
agentctl checkpoint W2
agentctl resume W2
agentctl retry W2
agentctl stop W2
agentctl inspect W2
```

- `start`: check preflight, ticket ownership, worktree cleanliness/branch, lock, model, container capability; start **new** Kiro session and persist actual session ID.
- `status`: show running/paused/blocked/completed/failed, ticket, branch, session ID presence (do not leak secrets), last checkpoint and gate.
- `logs`: show redacted per-worker output, event timestamps, last verified exit code.
- `checkpoint`: persist state record, worktree diff/hash, test evidence references, and current ticket progress; do not commit behind orchestrator.
- `resume`: require same session ID, logical working directory, isolated state volume and no active process; continue previously saved Kiro session.
- `retry`: return a failed ticket to the worker with concrete failure evidence after orchestration review. Preserve prior state; bounded attempt count (initially e.g. 2 automatic repair attempts, then escalate).
- `stop`: graceful termination and checkpoint, then forced stop only if necessary; never remove worktree/session by default.
- `inspect`: inspect container, UID, mount policy, active command, lock status and audit log location.

## State machine

```text
NEW --start--> RUNNING --finish--> COMPLETED
                  |  |               |
                  |  +--error------> FAILED --retry--> RUNNING
                  +--pause/stop----> PAUSED --resume--> RUNNING
                  +--needs owner---> BLOCKED --approved fix--> PAUSED
```

Completed **worker session** is not completed **ticket**: the orchestrator must verify acceptance and gates. A process exit code of 0 is not proof of code correctness.

## Persistent per-worker state

Keep under a **non-committed** private orchestration directory outside release artifacts, e.g. `.orchestration/state/W2.json` and `.orchestration/logs/W2/`, excluded from Git.

Required fields: schema version, worker ID, ticket ID, feature branch, absolute/logical workspace path, Kiro harness (e.g. V3), configured model ID, effort, exact session ID, container image reference, container ID, PID/lock, start/end timestamps, status, attempt count, last checkpoint description/hash, last exit code, relevant test/gate evidence paths, and sanitized error.

Write changes **atomically** (temporary file + rename), track an event journal, validate schema before use. One writer per worker; reject stale locks only after confirming the old process is gone. Never log credentials or full environment variables.

## Kiro CLI behavior to rely on only after G0

Official Kiro documentation (as of 2026-11-11) describes:

- `kiro-cli chat --list-models --format json` for available local model IDs;
- `kiro-cli chat --v3 --no-interactive --agent <name> --model <verified-id> --effort medium ...` for a pinned headless worker;
- `--resume-id <exact-id>` to target an existing local session;
- `--output-format stream-json` for JSON-lines events; and
- `kiro-cli chat --list-sessions` to inspect sessions in the current directory.

These are **documented flag capabilities**, not proof that this environment has appropriate accounts or installs. Session IDs must come from actual Kiro output or verified session listing; do not guess or parse a human-oriented hint as the only persistence path. Pass a prompt through stdin or as a positional argument (not both). Prefer least-privilege `--trust-tools`/permissions rules rather than `--trust-all-tools`; for headless runs, tool approvals must be configured ahead of time.

Typical G0 conceptual invocation (requires correct image, model ID, auth, permission policies; **do not run unsandboxed**):

```text
kiro-cli chat --v3 --no-interactive --agent <configured-agent> \
  --model <verified-haiku-5.5-id> --effort medium \
  --output-format stream-json <approved-prompt>
```

Resumption uses matching harness/session and persists the worker's Kiro home database **and** stable working directory across restarts. Kiro sessions are scoped by directory; `--resume` (most recent) is unsafe for orchestration. Use exact `--resume-id`, with an active-session lock.

## Interrupted/partial output handling

Only a complete structured terminal event plus successful exit and verified files can mark a worker run finished; interruption records/stale processes remain paused or failed. Checkpoint first, then restart or resume. Revalidate diff against allowed paths before merging; do not trust a resumed agent's prior claims of tests.

## Acceptance and clean-up

The utility's own smoke tests cover start/status/logs/stop/resume/retry, missing session, corrupt state, stale lock, two simultaneous resumes, lost container, and auth outage. Orchestrator approves only when fail-closed behavior and persistence are proven. Destructive cleanup of volumes/worktrees/logs is a separate owner-approved action.
