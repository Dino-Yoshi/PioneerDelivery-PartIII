# W1 — Foundation, CLI, Tooling, and Packaging Brief

**Worker:** Kiro CLI / Claude Haiku 5.5 Medium, in its own restricted container and feature/foundation worktree. **Supervisor:** Claude Code/Sonnet orchestrator. **Work tickets:** `SETUP-01`, `CORE-01`, `OPS-01` plus packaging mechanics requested by `UX-01`.

## Mission

Make the project buildable, discoverable, and safe for other workers to implement independently. Your highest-value result is a **stable interface contract** that lets W2–W4 write algorithms without modifying shared infrastructure. Separate product code from internal orchestration tools.

## Mandatory sources

Read `reference/PA-PartIII-2026Fall.pdf`, `docs/00_REQUIREMENTS_TRACEABILITY.md`, `docs/01_SYSTEM_DESIGN.md`, `docs/05_CLI_CONTRACT.md`, `docs/06_REPOSITORY_AND_STYLE.md`, `docs/08_ISOLATION_AND_SECURITY.md`, `docs/09_WORKER_RECOVERY.md`, and assigned ticket file(s).

## Contract

- C++20 + CMake/CTest, **one** executable `pioneer`, commands `schedule`, `knapsack`, `cleanup`.
- Distinct algorithm headers and sources, no DP in `main.cpp`/parser.
- Task A: signed int64 time; fixed-point int64 cents parsed from nonnegative decimal with <=2 fractional digits.
- Task B: positive integer fuel, nonnegative integer profit, nonnegative budget.
- Task C: lowercase nonempty source/pattern, sorted distinct original zero-based eligible indices.
- File/stdin input, stable machine key-value mode, default human-readable output, useful root/subcommand help, strict errors.
- Self-contained C++ tests under CTest; the agent framework is not a runtime dependency.

## Responsibility sequence

1. After G0 passed by orchestrator, establish independent Git/CMake/CTest skeleton; never independently publish/push.
2. Define minimal typed inputs/results and header signatures; ask orchestrator to freeze them at G1.
3. Implement validated parsing/help/formatting while leaving algorithm functions as clearly isolated dependencies.
4. Set up `agentctl` as a minimal safe internal wrapper **only after** the host/container/session mechanism was confirmed at G0; no overbuilt agent platform.
5. Supply quick-start example fixtures, minimal Linux release packaging support; W5 measures usability separately.

## Checks before reporting success

Clean configure/build, CTest smoke; `--help` and invalid-option behavior; example grammar parsing, `--input` vs stdin, `--machine` exact keys; no writable shared Git metadata in worker container; `agentctl` lock/start/resume safe smoke; no secrets in logs.

## Forbidden changes and handoff

Do not implement DP recurrences or edit worker-owned algorithm test files. Do not change model/effort/container security. Report header signatures, proposed parser rules, exact changed paths, build commands/exits, test evidence and unresolved blockers to Sonnet. **Do not commit/merge/push.**
