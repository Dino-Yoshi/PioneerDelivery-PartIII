# Gate Evidence Record — Cross-platform check (Windows, macOS, Linux)

- **Status:** PASS for source build, tests and CLI use on Linux, macOS and Windows
- **Date/time and timezone:** 2026-11-11 18:36 PST
- **Requirement:** owner instruction that the program must also work for graders on Windows and macOS
- **Branch/commit/environment:** `integration` 1a15d61, GitHub Actions run 43142383981 on GitHub-hosted runners (private repository)

## What was checked

| Check | Evidence | Result |
|---|---|---|
| Source audit for platform-specific code and non-ASCII bytes | grep of `src/`, `include/`, `tests/`, `CMakeLists.txt` | none found; the CLI check script already handles the Windows null device |
| Real Windows input hazards | New code in `src/main.cpp` (UTF-8 BOM ignored, UTF-16 rejected with a clear message) and `tests/CliChecks.cmake` cases for BOM (file and stdin), CRLF and UTF-16; the raw bytes written by CMake were checked with `od` (`ef bb bf`, `ff fe`) | pass locally and on all three runners |
| Ubuntu | GCC 15.2.0, CMake 4.4.5, default generator | configure, build, 9/9 tests, six `--machine` example runs (three via `--input`, three via stdin), exit codes 2 and 1: pass; 0 warnings |
| macOS | Apple Clang 21.0.0 on arm64 (Apple Silicon), Darwin 25.6.0 | same: pass; 0 warnings |
| Windows | MSVC 19.51 (Visual Studio generator, Release), exe at `build/Release/pioneer.exe` | same bash checks pass; PowerShell run of all three commands including a piped `Get-Content` input exits 0 with correct output; `cmd` run with `<` redirection exits 0 with correct output; 0 compiler warnings |

## Limits

- Tested compilers: GCC, Apple Clang and MSVC only. MinGW, Intel Macs, 32-bit systems and older Visual Studio versions are not tested.
- The CI run exercises the source build and the examples. It does not time a cold start on Windows or macOS and does not produce a prebuilt Windows or macOS package; those users build from the source ZIP with the commands in `README.md`.
- Line endings of the program's own output on Windows were not examined; the CI bash checks strip carriage returns before comparing.

## Decision

PASS on the evidence above.
