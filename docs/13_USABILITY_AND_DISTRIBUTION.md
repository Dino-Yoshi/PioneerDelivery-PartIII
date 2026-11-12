# 13 — Five-Minute Usability and Distribution Specification

## User requirement

A first-time user should be able to **obtain, extract/install, discover commands, execute each algorithm, and interpret results within five minutes** on a **clearly supported prebuilt target**. This is a measurable release acceptance target, not a promise of zero setup on every operating system.

## Two independent distribution paths

| Path | Audience | Tool dependency | Objective |
|---|---|---|---|
| **Quick-start release archive** | Everyday user / demo | No compiler, CMake, LaTeX, Kiro, or Claude Code | <=5 minute onboarding on tested Linux distro/architecture |
| **Source ZIP (Canvas submission)** | Professor / developer / grader | Documented C++20 compiler, CMake/CTest; LaTeX only to regenerate PDF | Reproducible build/test using exact commands |

Neither path should download AI models or require agent orchestration. Provide executable when reasonable for the tested target; do not claim generic portability for a binary built against a specific libc/runtime.

## Target release archive (Linux first)

```text
pioneer-linux-<arch>/
├── pioneer                  # tested executable
├── README.txt or README.md  # start here; <1 page quick-start
└── examples/
    ├── schedule.txt
    ├── knapsack.txt
    └── cleanup.txt
```

Use tested architecture/OS/runtime labels on archive name; don't misrepresent portable static binaries unless verified. Avoid symlink-only packaging and runtime dependence on source tree.

## Successful cold-start journey

1. Retrieve the published archive (or make an offline archive available for timed tests), extract in a normal writable directory without admin privileges.
2. Read a short start-here section, run `./pioneer --help`.
3. Run `./pioneer schedule --input examples/schedule.txt` and interpret max profit and selected IDs.
4. Run `./pioneer knapsack --input examples/knapsack.txt` and interpret profit/fuel/IDs.
5. Run `./pioneer cleanup --input examples/cleanup.txt` and interpret deletion count/original indices.
6. Optionally try `--machine` and task-specific `--help`; these should not need separate tutorials.

`--help` must be concise and runnable. Human-readable output is default; avoid dense traces/debug tables. Error messages should suggest valid usage rather than forcing users to read source code.

## Measurement of G5-U

Use a genuinely clean VM/container/host representing the claimed Linux target. The user should have ordinary shell/extraction capability but not project build tools installed. Record:

- platform (distribution, version, CPU architecture, relevant libc/runtime), package name/hash;
- start event (archive acquisition begins), stop event (third example result interpreted); elapsed time and whether <=300 s;
- successful outputs for all three examples and clarity of explanations;
- any commands user needed, unexpected permissions, missing libraries, searches/documentation consulted, confusing options;
- rerun after usability fixes.

The evaluator should not rely on unmentioned local tools or developer-specific environment. The timing must be **actually measured**, not estimated from the number of documented steps.

## Source build usability

Source `README.md` should also expose short copy-paste Linux commands to configure/build/run/test, with required compiler and CMake versions. It is fine if provisioning a machine with no compiler exceeds five minutes; the prebuilt archive is the path meeting the strict timing objective. Both paths remain supported and documented accurately.

## Ownership

W1: help, CLI ergonomics, example files, prebuilt packaging mechanics. W5: README quick-start, outsider-style timed walkthrough and issue reports. Sonnet: enforces G5-U, independently checks archive, obtains final owner signoff. `UX-01` owns the acceptance evidence.
