# 15 — Orchestrator Review Procedure and Worker Handoffs

## Principles

A worker's deliverable is a **proposal for acceptance**. Sonnet owns the integration decision; it must not behave as a passive log relay. Every ticket is accepted because concrete evidence satisfies its criteria, never because an agent reports "done".

## Worker brief (before execution)

Give each worker one ticket at a time and **only** the relevant specification and contract. Include:

- Exact ticket ID and worker role/worktree (e.g. W4 `feature/cleanup`), frozen interface revision and allowed file paths.
- Links/paths to the authoritative PDF and task-specific algorithm spec; explicit required complexity and reconstruction invariants.
- Concrete input/output tests and independent evidence expected; no unapproved changes to shared headers or CMake.
- Human-centered C++ conventions, prohibition on additional dependencies or alternate production algorithms.
- Expected output: changed files, decisions, test commands/results, edge cases, unresolved questions, no commit/merge/push.

Use `docs/templates/WORKER_HANDOFF.md` as a structured prompt template. Workers must not infer prior conversational context they cannot access; the brief must be self-contained.

## Accept/reject review checklist

- **Scope:** only assigned files edited, no deleted fixtures, no secrets/paths introduced.
- **Correctness:** independently reason through state/recurrence/traceback; run provided and adversarial cases.
- **Complexity:** actual sorting/binary search/table dimensions fit the PDF; no hidden exponential algorithm in production.
- **Determinism:** tie behavior consistent; result ordering unambiguous.
- **Numerics:** cents parsed exactly, sums checked, table sizes validated, no silent truncation.
- **Readability:** clear names, straightforward loops, meaningful comments, no needless architecture.
- **Testing:** tests have meaningful assertions and do not merely echo the production algorithm. Check actual exit codes.
- **Documentation:** record only observed behavior, keep README/test report synchronized with implementation.

If any category fails, reject the ticket with **one targeted actionable remediation**, including failing input, observed vs expected result and file/line if known. Avoid vague re-prompts and retries without evidence.

## Review independence

W5 must implement oracle reasoning based on the **problem definitions**, without copying W2/W3/W4 recurrence code. Sonnet then reviews both production and oracle separately. Even independent exhaustive oracles can be wrong: manually check a subset of small cases, especially endpoint compatibility and original-index semantics.

## Integration change protocol

When a worker discovers a shared contract problem: report to orchestrator, propose minimal change, and identify affected consumers/tests. Orchestrator approves, assigns owner W1 (or a narrow corrective ticket), updates docs, coordinates a single integration merge and requests relevant worker adaptation. No simultaneous ad-hoc edits to shared headers.

## Work reporting format

Every completed worker turn should report: worker/ticket, files changed, algorithmic decisions, commands **actually run**, test count/status, unfinished issues, potential contract conflicts, and checkpoint/session ID reference. Progress updates and completion reports are different: incomplete work remains in-progress.

## Human-facing status

Sonnet reports concise phase summaries to the user: accepted tickets, failures, current gate status, deviations needing approval, and next step. The human should not have to inspect five independent conversations to understand progress. Do not overwhelm the user with raw logs; provide paths to evidence and state blockers explicitly.
