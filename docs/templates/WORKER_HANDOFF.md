# Worker Ticket Dispatch Template (fill before launching)

**Worker ID:** W__
**Ticket:** __
**Claude Code orchestrator:** Sonnet 5.5 Medium
**Kiro model/effort:** Haiku 5.5 Medium (verified model ID: __)
**Assigned branch / worktree:** __
**Allowed paths:** __
**Forbidden paths:** __
**G1 interface revision / commit:** __
**Reference assignment:** `reference/PA-PartIII-2026Fall.pdf`
**Required task spec:** __
**Ticket file:** __

## Objective

[bounded outcome, not vague whole-project assignment]

## Acceptance conditions

[copy exact criteria from ticket, include DP/time/space and reconstruction properties]

## Required checks and evidence

[fixtures, actual build/CTest commands, expected/actual results, branch/file scope]

## Agent restrictions

Edit only assigned paths. No Git commits, merges, pushes, tool/model/security changes, new dependencies, or cross-worker communication. Report blockers and interface change requests to the orchestrator. Do not claim tests run unless actually executed.

## Delivery message format

Ticket/status; changed paths; design decisions; actual command/exit outputs; test summaries; known failures; recommended next gate; session ID/checkpoint reference.
