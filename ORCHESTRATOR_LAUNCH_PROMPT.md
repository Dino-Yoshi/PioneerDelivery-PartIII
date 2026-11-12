# Copy-Paste Launch Prompts for Claude Code

Start Claude Code from the project root that contains **`CLAUDE.md`**, using the chosen Sonnet 5.5 / Medium configuration.

## First pass: review without writing product code

> Read the root `CLAUDE.md`, `START_HERE.md`, the authoritative `reference/PA-PartIII-2026Fall.pdf`, and the linked design/ticket documents. Treat this as a frozen design handoff. First report: (1) requirements and user-selected choices, (2) exact agent hierarchy and worktree/container isolation, (3) ticket execution order and quality gates, (4) G0 checks you can perform safely, (5) any genuine environment blockers. Do not implement the product, run worker agents, push, or release until I authorize execution.

## Later: authorize development when ready

> I authorize development of Pioneer Delivery Part III using the frozen contract in `CLAUDE.md`. Begin with G0 preflight and fail closed if the selected Kiro/Haiku Medium worker configuration or container isolation cannot be established. On a passing G0, implement the foundation and gate G1, then delegate scoped tickets to five Kiro workers as permitted by the dependency graph. Maintain exact session checkpoints and evidence, review and commit only verified work, integrate incrementally, and leave final main promotion, remote push, and Canvas submission pending my explicit approval. Report gate results and blockers truthfully.

These are **prompts, not evidence that development was started**. The first prompt is safe to use immediately for planning; the second intentionally authorizes execution.
