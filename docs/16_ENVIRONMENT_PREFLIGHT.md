# 16 — G0 Environment Preflight (Before Implementation)

The handoff was prepared outside the intended developer machine. **It does not establish** that Claude Code, Kiro CLI, rootless containers, API-key headless auth, compiler versions, or access to model names are configured on the target system. Verify them before claiming readiness to execute.

## Mandatory command/capability inventory

| Component | Suggested safe check | Evidence / policy |
|---|---|---|
| Claude Code | `claude --version`; inspect selected model/effort in CLI | Sonnet 5.5/Medium confirmed by actual setup |
| Kiro CLI | `kiro-cli --version`, `kiro-cli chat --list-models --format json` | Identify **actual model ID** for Haiku 5.5; do not invent identifier |
| Kiro headless | Confirm `KIRO_API_KEY` presence securely; run a harmless V3 `--no-interactive` prompt in container | No secret value printed; capture session ID and output |
| Kiro reasoning effort | Check `--effort medium` with selected model in smoke run | Medium supported by account/model |
| Git | `git --version`, `git worktree list` after setup | Branch/worktree support, safe permission boundaries |
| Toolchain | `c++ --version`, `cmake --version`, `ctest --version` | Available C++20 compiler and CMake/CTest |
| Containers | `podman --version` or `docker --version`; verify rootless mode | Non-root, mount/network/limit enforcement |
| LaTeX | `latexmk -v`, `pdflatex --version` | Required packages available for report compilation |
| ZIP tool | `zip -v` or equivalent | Source archive can be assembled and extracted |

Model names in design are **product choices**. Local Kiro model ID strings and Claude Code model selector values are version/account specific. Pin only after listing and verifying support. If Haiku 5.5 Medium isn't available, **BLOCK** and ask owner; don't auto-switch to another model. Similarly, `--v3` is an explicit recommended harness for Kiro headless workflows, not proof that installation/auth works.

## Safe start order

1. Verify available executables/version/permissions. No source modifications yet.
2. Validate `KIRO_API_KEY` provisioning for headless mode; never request user to paste secrets into prompts or commit them.
3. Build and inspect an image only after confirming rootless container runtime; test isolated workspace mounts and false-access denial.
4. Invoke a harmless Kiro prompt in container using explicit harness, model ID, `--effort medium`, approved tool permissions and structured output. Confirm process exits and logs.
5. Confirm exact Kiro session ID capture and resumption after stopping/recreating container with persistent session state and same logical working directory.
6. Record preflight details in G0 evidence; do not mark G0 PASS if any required safeguard is unverified.

## Practical hazards

- Kiro's official headless documentation uses `KIRO_API_KEY`; an interactive login may not be equivalent. An account can support interactive Kiro without granting required headless key access.
- CLI flags and model identifiers change across versions. Inspect real `--help` and model listing; update the internal wrapper commands only after real checks.
- Kiro stores conversation sessions by directory. A container path that changes between restarts can lose apparent session visibility even if database persisted.
- Build images should pin/compiler/CLI dependencies, but the container orchestration stack is **not** required to run the end-user application.
- If networking restrictions prevent API/model access or container image setup, report G0 as BLOCKED. Do not downgrade to host-wide unrestricted tool execution as an invisible workaround.

## Approved fallback policy

A fallback that changes the selected workers, remote execution, sandbox strength, security posture, or agent authority is a **human decision**. Orchestrator may propose alternatives with trade-offs but must not activate them automatically. For a minor version-specific flag difference that preserves capabilities/security, it may adapt and record the change.
