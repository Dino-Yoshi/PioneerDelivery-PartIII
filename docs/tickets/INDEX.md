# Ticket Index — Pioneer Delivery Part III

**ACCEPTED: SETUP-01, CORE-01 (G1); A-01, A-02, B-01, B-02, C-01, C-02 (G2, G3); TEST-01 (G3); DOC-01 (skeleton, G2). SKIPPED by owner: OPS-01. NOT STARTED: DOC-02, UX-01, RELEASE-01.** Statuses are updated only after verified evidence.

| Ticket | Owner | Depends on | Goal |
|---|---|---|---|
| [SETUP-01](SETUP-01.md) | W1 | none | Repository and build foundation |
| [CORE-01](CORE-01.md) | W1 | SETUP-01 | Shared interfaces, CLI parsing, output, help |
| [OPS-01](OPS-01.md) | W1 | SETUP-01 | Isolated worker lifecycle and resume utility |
| [A-01](A-01.md) | W2 | CORE-01 | Scheduling preprocessing and DP |
| [A-02](A-02.md) | W2 | A-01 | Scheduling reconstruction and regression tests |
| [B-01](B-01.md) | W3 | CORE-01 | 2D knapsack DP |
| [B-02](B-02.md) | W3 | B-01 | Knapsack reconstruction and regression tests |
| [C-01](C-01.md) | W4 | CORE-01 | 2D subsequence/deletion DP |
| [C-02](C-02.md) | W4 | C-01 | Original-index traceback and cleanup tests |
| [TEST-01](TEST-01.md) | W5 | CORE-01; algorithm results for final runs | Independent exhaustive oracles and integration verification |
| [DOC-01](DOC-01.md) | W5 | SETUP-01 | README and LaTeX report skeleton |
| [DOC-02](DOC-02.md) | W5 | TEST-01; A-02; B-02; C-02 | Final Task D test evidence, PDF report, and attribution |
| [UX-01](UX-01.md) | W1 + W5 | G4 | Five-minute first-run packaging and usability verification |
| [RELEASE-01](RELEASE-01.md) | Orchestrator | all prior tickets + G5/G5-U | Integrated release, source ZIP and human signoff |
