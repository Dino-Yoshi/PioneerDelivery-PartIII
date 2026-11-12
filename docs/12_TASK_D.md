# 12 — Task D: README, Test Evidence, LaTeX PDF, Attribution (20 points)

This is a deliverable specification, not a finished README, report, or results sheet. Task D is 20% of the grade and must advance alongside implementation.

## D1: Practical README (5 points)

The **product** `README.md` opens with a direct, copy-paste Quick Start. Include: project purpose, each task's purpose, language and tested compiler/C++ standard, dependencies and supported platforms, source build commands, test commands, input grammar, `--help`, file/stdin usage, human vs machine output, and at least one **working invocation for each task**. Link to the LaTeX report and test evidence; do not make the user read either to run the app. Avoid filling the README with internal agent infrastructure.

Follow the accessible existing Pioneer Delivery documentation conventions (direct task-oriented organization, honest test tables). If earlier source styling is not accessible, use a restrained academic/engineering style; do **not** claim to have inspected it when unavailable.

## D2: Test cases and **actual** results (8 points)

Provide official example plus >=3 extra cases per A/B/C; all specified edge classes. Record **input, expected output and actual output for every named test**, with test IDs and CTest outcomes. At least one small A and one small B optimum independently checked by exhaustive subset enumeration; for C verify that reported deletion indices leave pattern a subsequence. We plan extra independent oracle tests for C as well.

Use `docs/TestResults.md` with tables backed by actual logs/fixtures. Never copy expected values into the `Actual` column until the program runs. Do not assert unique optimal IDs where the problem permits ties.

## D3: LaTeX-authored report compiled to PDF (5 points)

Author in `docs/AlgorithmReport.tex`, compile to `docs/AlgorithmReport.pdf`. Recommended short report structure:

1. Title, purpose, language/testing environment and compact introduction.
2. Task A — DP state, base, recurrence, fill order, predecessor search, traceback, O(n log n)/O(n), greedy counterexample.
3. Task B — 2D state, base, recurrence, fill order, traceback, O(nB) time/space, why pseudo-polynomial.
4. Task C — 2D prefix state, impossible states, transitions, fill order, original-index traceback, O(nm), valid-subsequence verification.
5. Test results summarized with stable references to `docs/TestResults.md` and independent oracle methodology.
6. **One genuine lesson learned or challenge encountered**, written **after implementation/testing**, or a truthful challenge already encountered in design if that is what the human confirms. Do not fabricate debugging incidents or assign personal reflections to the user without review.

Keep report concise (rough goal 4–6 pages if content fits; no page limit in PDF). Make equations and labels legible. Use `latexmk -pdf` when available, inspect resulting PDF visually, and keep LaTeX dependencies documented and commonly available. The final source archive may include both `.tex` and `.pdf`; the rubric explicitly requires the PDF.

## D4: Clean source ZIP and contributor disclosure (2 points)

Include source code, CMake setup, product README, runnable tests and fixtures, actual test results, final PDF report, and any files needed for reproducible build. Exclude generated `build/`, sensitive credentials, agent sessions, temp logs and internal container caches.

**Human and original developer involvement:** include explicit, factual attribution. If project owner and original developer are the same person, say so; do not invent a second contributor. Human involvement may include original concept, problem interpretation, architecture selection, algorithm discussions, independent review, testing/acceptance and final submission—attribute only roles the person truly performed.

**AI tools:** explicitly disclose Claude Code (Sonnet orchestrator), Kiro CLI (Haiku workers), and any other actually used tools, with **specific activities** (requirements analysis, code assistance, tests, docs, review). Update disclosure based on work performed, not intended work. Independently verify submitted code and explanations as the PDF requests.

### Attribution model (fill with owner-confirmed facts)

```text
Development and attribution
- Original Pioneer Delivery developer: [owner to confirm identity and contribution]
- Part III human project owner/developer: [owner to confirm; may be same person]
- Human responsibility: final architecture/algorithm choices, review,
  acceptance of test evidence and release (verify actual roles before publishing).
- AI assistance: [actual tools/models, concrete work, when used].
- Verification: [actual CTest/oracle evidence and review performed].
```

Do not insert fabricated co-author credit in Git history. Respect any institution-specific AI policy; the provided PDF explicitly allows disclosed generative AI assistance and requires independent verification.

## Deferred until development

- Actual compiler/version and runtime support results.
- Actual test case outputs, failure logs, CTest counts and seeds.
- Genuine lessons learned (human confirms wording).
- Actual contributor identities and tooling work performed.
- Final PDF compilation, ZIP archive manifest and checksum.

The structure can be completed early; these facts cannot be declared finished now.
