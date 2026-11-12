# 10 — Quality Gates and Evidence Rules

**Policy:** A gate is PASS only if its required checks actually executed and the evidence is recorded. Design approval is not execution evidence. States: `PENDING`, `PASS`, `FAIL`, `BLOCKED`, with timestamps and responsible reviewer. Report failures rather than substituting claims.

## G0 — Environment readiness and isolation (before worker coding)

**Owner:** orchestrator with W1 support.

Acceptance:
- Confirm installed Claude Code and intended Sonnet 5.5 / Medium setting.
- Confirm Kiro CLI install/version, available model ID for Haiku 5.5 and medium effort, compatible harness, appropriate headless auth entitlement/`KIRO_API_KEY`, noninteractive tool permission policy.
- Verify C++20 compiler, CMake, CTest, Git/worktrees, `latexmk`/LaTeX packages for required PDF, zip packaging, rootless Podman/Docker.
- Confirm worker container filesystem restrictions, no common Git metadata/host credential access, resource caps, redacted logs and Kiro session-state persistence.
- One harmless **real** containerized headless Kiro invocation; capture session ID and verify resume after container restart.

Evidence: version reports, model-list proof without secrets, non-destructive smoke log, isolation denial checks, credential **presence** only (never value), selected resource limits. **No implementation begins if G0 fails** without explicit owner decision.

## G1 — Foundation / interfaces

**Owner:** W1 builds, orchestrator reviews.

Acceptance: clean CMake configure/build from new repository skeleton; `pioneer --help`/subcommand help behavior matches preliminary contract; headers/structures/interfaces reviewed; CTest registered; task-specific parser plans validated. Must allow algorithm agents to work independently without editing CLI or CMake. Freeze shared interfaces on `integration`.

Evidence: commit/hash, configure/build logs, CLI smoke outputs, interface review signoff. A stub must not be misreported as a completed algorithm.

## G2 — Each worker's implementation acceptance

**Owner:** assigned worker, orchestrator reviewer.

Acceptance: code limited to ticket scope, uses prescribed DP (not greedy/exhaustive/1D substitutions), correct state/base/recurrence/fill and reconstruction, required complexity, readable style, strict input/overflow where module owns it, dedicated tests pass. Worker supplies actual changed file list and commands/exits.

Evidence: reviewable diff, actual local tests, scope check and reviewer approval. Failing work stays on feature branch.

## G3 — Independent correctness verification

**Owner:** W5, independently verified by orchestrator.

Acceptance: official examples + >=3 additional tests per A/B/C; all four official C examples; exhaustive small-instance test-only oracles for A and B (and recommended C), multiple deterministic randomized seeds, invariants for reconstructed result; no test asserts one uniquely required ID set where multiple optima valid.

Evidence: seed list, case list, oracle construction notes independent of production recurrence, real expected/actual outcomes, actual CTest output/log paths. Do not copy algorithm code into oracle.

## G4 — Integrated build and regression

**Owner:** orchestrator.

Acceptance: review and merge accepted worker commits one at a time into `integration`, clean source build with documented compiler, full `ctest --test-dir build --output-on-failure`, all three CLI file/stdin examples, help, human + machine presentation, invalid-input behavior, cross-module link/symbol consistency. No regression in previously integrated tasks.

Evidence: integration commit, full command logs and exit codes, CLI output capture, reproducibility check. Do not label `main` ready until all later gates pass.

## G5 — Documentation and release audit

**Owner:** W5 documentation, orchestrator audit, human final approval.

Acceptance: README has actual working commands, language/compiler/dependencies, all task explanations/I/O/examples, test instructions; `docs/TestResults.md` has per-case inputs/expected/**actual** values; LaTeX report source compiles to readable PDF and covers DP states/base/recurrences/fill/traceback/complexity/test refs and one **genuine** lesson/challenge; correct human/original-developer and AI disclosures; no secrets or unverified claims; Canvas-compatible source ZIP built and checked.

Evidence: `latexmk` build exit, PDF render/inspection, README command replay, ZIP manifest/list, source archive clean-build result, disclosure review, human go/no-go.

### G5-U — Cold-start five-minute usability subgate

**Owner:** W5 evaluator, orchestrator release blocker.

Using a supported **tested prebuilt Linux** archive on a clean suitable Linux environment with no compiler/CMake/AI tools required: time from obtaining/extracting the package to finding help, running all three examples, and understanding reported results. Record timer start/stop, host distro/architecture/runtime constraints, actions, friction and result. Target: **<=5 minutes**. If the tested platform cannot run the chosen binary, fix packaging or narrow support claim. The source ZIP additionally needs accurate source-build steps; its setup duration on a machine lacking tools is not guaranteed <=5 minutes.

## Gate failure/retry policy

- A **FAIL** remains until the responsible worker fixes and tests it; review before retry, do not brute-force indefinite autonomous attempts.
- A **BLOCKED** gate (missing credentials, denied container isolation, unavailable model) needs visible owner decision for material policy changes.
- Never bypass a later gate due to passing G2, and never promote to main on worker-reported success alone.
- Every merged change that touches a verified contract triggers relevant regression checks; major shared change reruns full G4.

## Evidence templates

Use `docs/templates/GATE_RECORD.md` and `docs/templates/WORKER_CHECKPOINT.md`. Records are completed **during actual execution**; this handoff intentionally has no PASS records.
