# 17 — Frozen Decisions, Deferred Facts, and Change Control

## Frozen design choices, made by the human owner

| Topic | Decision |
|---|---|
| Source relationship | New independent Part III Git repository, not a continuation branch |
| Language / build | C++20, CMake, CTest; self-contained C++ test executables |
| CLI architecture | One `pioneer` binary, `schedule`, `knapsack`, `cleanup` subcommands |
| Input | Task-specific plain text via stdin/file; strict validated parsing |
| Output | Human-readable by default plus stable `--machine` key-value mode |
| Task A | DP with binary-search predecessors, finish-time sorting, p(i) traceback, tie excludes current |
| Task B | Mandatory 2D bottom-up DP, previous-row transitions, exclude on tie |
| Task C | Mandatory 2D matching/deletion DP, explicit impossible states, original-index traceback |
| Testing | Official examples, required edges, independent small-instance brute-force test oracles, deterministic regressions |
| Numbers | Task A time int64; profit exact decimal cents in int64 with max two fractional digits; B fuel/profit integer |
| Error strategy | Descriptive stderr + nonzero exit, no silent correction |
| Style | PascalCase types, camelCase functions/variables, human-readable straightforward code, purposeful comments |
| Modularity | Focused headers/source files and thin CLI; no unnecessary class hierarchy |
| Documentation | Concise user README, actual Markdown results tables, LaTeX report compiled to PDF |
| Packaging | Clean source ZIP for Canvas, no build directories; prebuilt tested Linux quick-start release as extra usability deliverable |
| Attribution | State human project owner and original developer contributions accurately; disclose real AI-tool activities |
| Dev agent hierarchy | Claude Code Sonnet 5.5 Medium sole orchestrator, five Kiro CLI Haiku 5.5 Medium workers |
| Isolation | Per-worker rootless containers and worktrees; Git reviewed/committed/merged by orchestrator |
| Branches | `main` release-only; `integration` staging; dedicated feature branches |
| Recovery | Simple persistent `agentctl` wrapper with exact Kiro session IDs, checkpoints, logs and locks |
| Gates | G0–G5 with G5-U five-minute cold-start acceptance, no unverified PASS |
| Scope constraint | No product code until owner authorizes execution |

## Defaults requiring local verification rather than design debate

- Repo name / checkout location: suggested `PioneerDelivery-PartIII`; **no absolute filesystem path was finalized**. Use owner-configured location rather than guessing.
- Exact Kiro local model ID, Claude Code selection and use of Medium effort; verify tool/account availability.
- Exact compiler/CMake/LaTeX versions supported; record from G0 and clean-room builds.
- Rootless Podman vs rootless Docker: prefer Podman if available; actual isolation is the requirement.
- Exact CI/image packaging and Linux target/runtime: choose based on measured environment and G5-U.
- Container resource limits and DP memory caps: choose explicit safe values based on actual hardware, document/test them.
- Final real contributor names and whether original developer and Part III owner are the same person: owner confirms before public disclosure.
- Whether previous Pioneer Delivery style artifacts can be inspected: if not accessible, follow the agreed task-oriented aesthetic without claiming exact reproduction.

## Facts intentionally deferred until implementation

Actual source files and executable; completed tests and their output; compiled report and reviewed pages; measured onboarding time; genuine challenge/lesson from work; release hash; any actual AI contributions beyond current planning/design discussions.

## Change-control rule

If specification, interface, security, model, or rubric interpretations conflict: stop affected work; cite the source line/page, identify consequence, propose minimal alternatives, and seek owner approval for **material** deviations. Record date, rationale and impact here. Worker suggestions are not automatically approved design changes.

## Known source oddity

The source PDF's C Example 4 explanation contains a stray paragraph repeating Example 1. The official numerical examples and C1–C4 requirements remain authoritative; do not silently reinterpret the input.

## Approved decisions and deviations log

All entries approved by the human owner on 2026-11-11 during the pre-execution interview.

| Date | Decision | Rationale / impact |
|---|---|---|
| 2026-11-11 | **Kiro auth in containers uses a private per-container copy of only the `auth_kv` login table from the host Kiro database, not `KIRO_API_KEY`.** Deviation from `16_ENVIRONMENT_PREFLIGHT.md`. | Kiro is already logged in on the host (GitHub login) and no headless key exists. Matches the owner's existing REA builder pipeline. Chat history is never copied. Credential presence, never value, is recorded at G0. G0 stays BLOCKED if the Podman sandbox escape test fails. |
| 2026-11-11 | **Container runtime is rootless Podman** (6.x). Docker is not used for workers. | Preferred by `16`. Rootless verified on the host before G0 sign-off. |
| 2026-11-11 | **Layout:** `p3/` is the `main` checkout. Worker worktrees live in `../p3-worktrees/`. Operational state (logs, checkpoints, Kiro session IDs, escape-test scratch) lives in `../p3-ops/`. Both are outside the repository. | Keeps writable worker mounts away from the repository's shared Git metadata (`08_ISOLATION_AND_SECURITY.md`) and prevents committing operational state. |
| 2026-11-11 | **Push policy:** everything except `main` may be pushed after gates pass. `main` is pushed only at G5 with explicit owner approval. | Remote `origin` is the private repository `Dino-Yoshi/PioneerDelivery-PartIII`. |
| 2026-11-11 | **Commit authorship:** author and committer are the owner's configured Git identity. Each commit carries the `Co-Authored-By: Claude Sonnet 5.5` trailer. Commits of worker-produced code name the worker, ticket and gate in the body. | Keeps history consistent with the Task D disclosure. |
| 2026-11-11 | **Resource limits:** at most 4 parallel workers, each limited to 3 GiB RAM, 2 CPUs and a process cap. **DP table cap in `pioneer`:** 256 MiB for Tasks B and C, a named constant with a boundary test, exceeding it gives an explicit error. Retry limit is 3 per ticket, then stop and report. | Host has 12 cores and 31 GiB RAM, shared with other work. Values to be verified as actually enforced at G0. |
| 2026-11-11 | **Prebuilt package:** static x86_64 Linux binary. G5-U cold-start trial runs in a clean Debian Podman container without a toolchain. Support claim is limited to x86_64 Linux as tested there. | Honest, reproducible and timeable by the orchestrator. |
| 2026-11-11 | **Attribution:** owner is sole human author and original developer. Disclose Claude Code (Sonnet 5.5), Kiro (Haiku 5.5) and any other tool actually used. State that the owner helped set up and plan the project and was present throughout, with implementation check-ins. Owner approves final wording at G5. | Satisfies the truthful-disclosure requirement of the assignment. |
| 2026-11-11 | **Kiro worker model ID:** `claude-haiku-5.5` appears in `kiro-cli chat --list-models`; effort via `--effort medium`. Flags are re-confirmed at G0. | Local verification, not a design change. |
