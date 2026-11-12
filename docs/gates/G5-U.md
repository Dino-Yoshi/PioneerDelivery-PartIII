# Gate Evidence Record — G5-U (cold-start usability)

- **Status:** PASS for the prebuilt x86_64 Linux archive, on a Debian 13 container. No human timed trial has been done yet.
- **Rerun note:** the archive was rebuilt and re-tested after the input-handling change (UTF-8 BOM accepted, UTF-16 rejected clearly). The hash below and the results in the table are from the rebuilt archive; the 32.3 s and 32.7 s outsider runs are the previous and the rebuilt archive respectively.
- **Date/time and timezone:** 2026-11-11 18:08 PST
- **Reviewer / worker:** W5-style evaluator (a new Kiro, Haiku 5.5 session acting as a first-time user), plus an automated clean-room run by the orchestrator
- **Branch/commit/environment:** `integration`; archive `pioneer-linux-x86_64.tar.gz`, SHA-256 `932510957cea3db619c56f769c6b8ffc039438b9ae681650e55c6bdc3548f802`
- **Requirement/tickets covered:** UX-01, G5-U in `10_QUALITY_GATES.md`

## Method and environment

- Package built by `tools/package-linux.sh` in the worker container (GCC 14.2, CMake 3.31.6). Static linking verified with `readelf` (no INTERP header) and by running the binary in a container that has no libc development files.
- Clean environment: `debian:trixie-slim` (Debian GNU/Linux 13, x86_64) with no compiler, CMake, make, ninja, git or Python, read-only root, no network, one CPU, 512 MiB memory. The `p3-clean` image used for the outsider trial adds only the Kiro CLI binary.
- Package size: about 2.1 MB binary; archive listing: `pioneer`, `README.txt`, `examples/{schedule,knapsack,cleanup}.txt`.

## Checks run

| Check | Actual action | Result | Evidence |
|---|---|---|---|
| Reproducible build | `tools/package-linux.sh` run twice on fresh copies (before the input-handling change) | archives byte-identical, same SHA-256 | `../p3-ops/verify/pkg*` |
| Windows-style inputs in the shipped binary | extracted archive in the clean container; files with a UTF-8 BOM, CRLF line endings, and a UTF-16 header | BOM and CRLF give `max_profit=5.00`; UTF-16 prints the clear message and exits 2 | terminal |
| Clean-room journey (scripted) | checksum, extract, `--help`, three examples, `--machine`, stdin, per-command help, unknown command | all exit codes as expected; 0 mismatches between the quick-start's claimed output and the real output; 0.05 s of command execution | `../p3-ops/verify/g5u/journey.out` |
| Outsider journey (timed) | A Kiro session with only the archive and its checksum, no compiler, followed the quick-start: verify checksum, extract, read `README.txt`, run help and the three examples, explain each result | success; all three results understood and checked by hand; wall-clock **32.7 s** (previous archive: 32.3 s) from container start to the session's final answer (includes model latency, sign-in and container start) | `../p3-ops/logs/W5E/`, `../p3-ops/verify/g5u-outsider/` |

## Friction found and fixed (reruns after each fix, all on the archive tested above or its predecessors)

1. First outsider attempt could not run the program: that was a permission-rule mistake in the orchestrator's test harness, not a product problem. It also exposed a real confusion: the cleanup explanation did not say which positions may be deleted. Fixed in the quick-start.
2. The quick-start did not say that a delivery finishing at time t may be followed by one starting at t. Added.
3. The cleanup input explanation of line 3 was misleading ("how many positions you are allowed to delete"). Reworded: it is the count of positions listed on line 4.
4. Remaining observations, not changed: schedule prints two decimals while knapsack prints integers (as specified in `05_CLI_CONTRACT.md`); the cleanup output prints indices without the pattern.

## Limits of this evidence

- The 32.3 s timing is from an AI session standing in for a first-time user, not a human. It measures a real end-to-end journey (obtain, verify, extract, read, run, interpret) but not human reading speed. A human timed trial by the owner would strengthen it and is recommended before release.
- Only Debian 13 x86_64 was tested. No claim is made about other distributions, other CPU types or other libc versions beyond the fact that the binary is statically linked.

## Decision

G5-U PASS on the evidence above, with the stated limits.
