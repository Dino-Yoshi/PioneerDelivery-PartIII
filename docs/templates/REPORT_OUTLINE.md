# LaTeX Algorithm Report Outline — Source Specification, Not Final Report

Target file after implementation: `docs/AlgorithmReport.tex`. Submission deliverable: compiled, visually checked `docs/AlgorithmReport.pdf`. Keep LaTeX source as well.

1. **Title/overview:** Pioneer Delivery Part III, author/contributors as confirmed, language/toolchain actually tested, brief algorithm objectives.
2. **Task A:** Input; state `OPT[i]`; base `OPT[0]=0`; recurrence; binary predecessor preprocessing; fill order; exclude-on-tie traceback; example and greedy counterexample; O(n log n)/O(n) analysis including sorting.
3. **Task B:** Input; 2D state `K[i][b]`; boundary values; recurrence; bottom-up row-major fill; previous-row restriction; reconstruction/budget proof; O(nB) time/space and numeric B pseudo-polynomial comment.
4. **Task C:** Input and eligible original indices; state `F[i][j]`; impossible sentinel; skip vs match transitions; fill order; original-index reconstruction and independent subsequence validation; O(nm) time/space.
5. **Testing:** numbered cases (refer to actual `docs/TestResults.md`), independent oracle methodology, actual observed outcomes, brief limitations.
6. **Challenge and lesson:** one genuine, owner-confirmed development reflection after implementation.
7. **Optional attribution references:** README carries primary detailed AI/human disclosure.

**Do not present a PDF as finished until LaTeX compiles and pages are visually inspected. Do not invent test outputs or a debugging story.**
