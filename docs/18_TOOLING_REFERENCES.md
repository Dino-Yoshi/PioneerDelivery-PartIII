# 18 — Public Tool Documentation Checked for This Handoff

Checked **2026-11-11**. URLs are references for the *orchestrator's G0 preflight*, not evidence that the target machine has these tools installed, the relevant plan, or a working API key. Verify current/local CLI help before scripting.

## Claude Code instructions

- [Claude Code: project CLAUDE.md memory and loading](https://code.claude.com/docs/en/memory)
- [Claude Help Center: give Claude context with CLAUDE.md](https://support.claude.com/en/articles/14553240-give-claude-context-claude-md-and-better-prompts)

A root `CLAUDE.md` is automatically loaded by Claude Code at session start. Keep it relatively compact; put granular procedures in linked documents and read relevant files before delegation.

## Kiro CLI execution, models and sessions

- [Kiro headless mode](https://kiro.dev/docs/cli/headless/): `--v3 --no-interactive`, explicit `--model` and `--effort`, `KIRO_API_KEY`, `--resume-id`, `--output-format stream-json`, and limited tool permissions.
- [Kiro CLI command reference](https://kiro.dev/docs/cli/reference/cli-commands/): `--list-models --format json`, `--list-sessions`, session selection.
- [Kiro session management](https://kiro.dev/docs/cli/chat/session-management/): directory-scoped persistent sessions and exact-ID recovery.
- [Kiro agent harness](https://kiro.dev/docs/cli/agent-harness/): pin explicit harness in automation instead of depending on evolving defaults.
- [Kiro available models](https://kiro.dev/docs/models/available-models/): product support for Claude Haiku 5.5 and Sonnet 5.5 (account/region restrictions may apply).

## User assignment

- `../reference/PA-PartIII-2026Fall.pdf`: professor's actual 5-page PDF. SHA-256 `56d5510955501d1d222642adbe54848887c13bf7f4c39505e69b58c35e1c4731`.
- No external tutorial or LeetCode solution supersedes the PDF's prescribed DP states and performance targets.

## Avoid command assumptions

- The installed Kiro model identifier may differ from the marketing model name. Query before pinning.
- A shell environment capable of `kiro-cli` interactively does **not** prove headless API-key capability.
- Kiro CLI's built-in command sandbox does not replace full worker container isolation.
- The `agentctl` commands specified in this design are **not native Kiro commands**; they are a proposed wrapper to implement after G0.
