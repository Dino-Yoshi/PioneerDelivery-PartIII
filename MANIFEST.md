# Handoff Contents

**Status:** authoritative planning and delegation materials; no product implementation or actual results.

| Location | Purpose |
|---|---|
| `CLAUDE.md` | Root Claude Code orchestrator contract; automatically loaded when launched from this directory |
| `START_HERE.md` | First-use instructions and design summary |
| `ORCHESTRATOR_LAUNCH_PROMPT.md` | Ready-to-copy review-only and later implementation-authorization prompts |
| `reference/PA-PartIII-2026Fall.pdf` | Original professor assignment, grading source of truth |
| `docs/00_REQUIREMENTS_TRACEABILITY.md` | A1–D4 obligations mapped to points, tickets, gates |
| `docs/01_SYSTEM_DESIGN.md` | C++ modular architecture and product boundaries |
| `docs/02_TASK_A.md` | Weighted scheduling state, binary predecessors, traceback, edges |
| `docs/03_TASK_B.md` | Required 2D knapsack state, recurrence, traceback, memory checks |
| `docs/04_TASK_C.md` | Required 2D deletion/subsequence alignment and original indices |
| `docs/05_CLI_CONTRACT.md` | Exact supported command, input, output, validation and help conventions |
| `docs/06_REPOSITORY_AND_STYLE.md` | C++20/CMake/CTest, repository tree, human-readable code style |
| `docs/07_AGENT_TOPOLOGY.md` | Orchestrator/workers, branches/worktrees, ownership and scheduling |
| `docs/08_ISOLATION_AND_SECURITY.md` | Rootless container boundaries, secret policies, mount safeguards |
| `docs/09_WORKER_RECOVERY.md` | `agentctl` design, persistent Kiro session IDs, state/locking |
| `docs/10_QUALITY_GATES.md` | G0–G5 and G5-U evidence/release conditions |
| `docs/11_TEST_PLAN.md` | Named official/adversarial cases, independent small oracles |
| `docs/12_TASK_D.md` | README, actual results, LaTeX PDF report, human/AI attribution |
| `docs/13_USABILITY_AND_DISTRIBUTION.md` | Five-minute tested Linux prebuilt quick-start target |
| `docs/14_RELEASE_PLAYBOOK.md` | Integration, clean ZIP, report, promotion and human approval |
| `docs/15_REVIEW_AND_HANDOFF.md` | Worker dispatch and orchestrator code-review standards |
| `docs/16_ENVIRONMENT_PREFLIGHT.md` | Real tool/model/auth/container checks required before execution |
| `docs/17_DECISIONS_AND_UNCERTAINTIES.md` | Locked human choices and intentionally deferred facts |
| `docs/18_TOOLING_REFERENCES.md` | Official external tooling documentation checked at handoff |
| `docs/tickets/INDEX.md` | 14 individual tickets and dependencies; all NOT STARTED |
| `docs/workers/` | Five independently scoped worker briefs |
| `docs/templates/` | Dispatch, evidence, result, reflection, README/report, attribution templates |

## Provenance

Prepared from the user's Part III PDF and the explicitly agreed design in this conversation. Source PDF checksum (SHA-256): `56d5510955501d1d222642adbe54848887c13bf7f4c39505e69b58c35e1c4731`.

The application name, build/test commands, CLI examples and branch names are **requirements for future implementation**, not current working artifacts. The user retains final signoff, and the orchestrator must obtain separate authorization before starting implementation or publishing work.
