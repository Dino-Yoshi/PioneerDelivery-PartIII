# W5 — Independent Verification, Documentation, and Usability Brief

**Worker:** Kiro CLI / Claude Haiku 5.5 Medium, isolated `feature/verification-docs` worktree/container. **Tickets:** `TEST-01`, `DOC-01`, `DOC-02`, UX-01 evaluation portion. **Start document skeleton and oracle fixtures while W2–W4 implement**, but mark any unexecuted outcome pending.

## Independence rule

Use **the original assignment PDF and task problem statements** to construct exhaustive small-instance oracles. Do not copy DP recurrence, predecessor routine, or traceback code from production modules. A oracle enumerates compatible subsets; B oracle enumerates feasible fuel subsets; C oracle enumerates eligible deletion subsets and runs an independent subsequence predicate. For each result verify optimum and separate reconstruction invariants.

## Test obligations

All named tests in `docs/11_TEST_PLAN.md`, at least four per A/B/C, including all four C examples; deterministic randomized cases and fixed seeds; tests registered with CTest; actual input/expected/actual/result evidence. Do **not** demand exact IDs for tied optimum unless specifically testing deterministic implementation tie policy.

## Documentation obligations

`README.md`: clear <5-minute first run path on supported prebuilt Linux release, purpose of each task, actual language/compiler/dependencies, compile/run/test commands, input/output, examples and help. `docs/AlgorithmReport.tex` and compiled PDF: DP states/base/recurrences/fill/traceback/complexities, A greedy failure, B pseudo-polynomial note, C impossible states, test references and **one genuine human-confirmed** lesson/challenge. `docs/TestResults.md`: actual data backed by logs. Human and original developer attribution and specific AI contributions should be accurate and owner-approved.

## Usability evaluation

Independently run the quick-start prebuilt archive on a clean **claimed** Linux environment; time from obtaining archive to executing/interpreting three examples; require <=300 seconds. Record platform, binary/archive hash, exact commands, elapsed time, friction/bugs. Do not declare five-minute usability achieved without an actual timed attempt.

## Boundaries

Own oracle and documentation files, not production algorithms, CLI parser or CMake. Report defects to Sonnet; do not quietly modify algorithm code to force tests to pass. Actual pass/fail counts, test logs, source changes and independent verification methodology go to orchestrator. **No commit/merge/push.**
