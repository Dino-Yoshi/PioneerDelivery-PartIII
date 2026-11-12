# 08 — Worker Containerization and Trust Boundaries

## Goal

The orchestrator may execute Kiro CLI worker sessions that edit code or run builds. Containers isolate worker processes and limit the damage from mistakes, while Git worktrees isolate development changes. **Worktrees alone are not a security boundary.** Container execution is development infrastructure only; end-user `pioneer` runs natively.

## Recommended baseline

- Rootless Podman preferred when available; rootless Docker is an acceptable alternative after G0 checks.
- Shared versioned image with the tested C++20 compiler, CMake/CTest, Kiro CLI, shell and minimal support tools. Pin image provenance/version rather than `latest` without verification.
- One container instance per worker with **non-root UID**, bounded CPU/memory/PIDs, working directory restricted to assigned workspace, isolated temporary and Kiro state volumes.
- Writable mounts: exactly assigned code workspace, worker-owned temporary/build/cache/session directory. Read-only mounts for task instructions, reference PDF, and approved dependencies when needed.
- **No** privileged mode, Docker/Podman socket, host home folder, SSH directory, personal repositories, host source tree outside assigned scope, host PID namespace, or unlimited mounts.
- Network: outbound only for documented Kiro API/auth and required dependency retrieval; prefer prebuilt images/dependencies so workers do not need unrestricted network.
- Credentials: inject Kiro headless API key securely at runtime, never in args, files committed to Git, logs, or images. Avoid exposing Claude Code orchestrator credentials to workers. Use least privilege and rotate/revoke after incident.
- Optional inner Kiro command sandbox/tool policies are defense-in-depth, **not a replacement** for container isolation. Headless trust-all mode is not approved by default.

## Git linked worktree nuance

A worktree's `.git` file may point to the shared Git administrative directory. If the worker receives access to that directory, it might alter other branches. Design a mount where the worker can edit tracked source files **without writable common Git metadata**:

- Prefer mounting only the worktree source directory with `.git` pointer inaccessible/nonfunctional inside the container, and do all Git operations **on the host under orchestrator control**; verify this with a write-denial test at G0.
- Alternatively create a temporary staging copy for each worker and have the orchestrator import a reviewed patch into its assigned feature worktree. Keep the original worktree isolated on the host.
- Never mount `.git` common dirs or a Git credential helper into a worker. A read-only `.git` pointer is not sufficient if the pointed-to metadata remains writable by another route.

## Required isolation smoke tests at G0

1. Confirm rootless operation, non-root UID, file ownership compatibility with the worktree, and resource limits actually enforced.
2. Worker can create/modify/delete a **temporary test file inside only its own assigned workspace**.
3. Worker cannot read or write another worker's workspace, the main/integration checkout, the host's private credential files, or Git common directory.
4. Worker can't access container engine socket or escape to host executables/filesystem.
5. Kiro authenticates in container and runs a harmless read-only headless prompt, using pinned available model/effort and explicit harness.
6. Worker Kiro session state survives container termination and can be resumed in the **same logical directory** with the exact session ID.
7. Confirm stdout/stderr/structured logs redacted and stored per worker; no sensitive env variables echoed.

A failing security check **blocks G0**. Do not quietly substitute an unrestricted host subprocess. Any fallback requires explicit owner approval and a documented risk/mitigation choice.

## Operational limits

- Use defined per-worker CPU/memory/PID caps, work duration cap, disk quota or monitoring, and an execution lock. Caps are adjustable based on measured compilation/DP test memory; never silently raise them to run unbounded code.
- Kill/stall detection should preserve worktree diff, logs, Kiro session state and last gate evidence.
- Deleting a failed container must not delete the worker's worktree or session volume.
- Avoid passing source files containing secrets in model prompts. Inspect generated logs before including them in final deliverables.

## Failure response

On permission denial, auth failure, missing Kiro headless entitlement, resource exhaustion, or unsupported runtime: mark the specific worker **BLOCKED**, preserve checkpoint/logs, report cause and safe alternatives, and wait for authorization if the change would weaken isolation or alter the selected model stack.
