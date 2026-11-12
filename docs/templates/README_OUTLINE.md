# Product README Outline — Source Specification, Not Final README

The final product `README.md` should **not** lead with multi-agent infrastructure; it addresses a first-time grader/user.

1. Title, one-line purpose, three algorithm subcommands.
2. **Quick Start (prebuilt supported Linux):** where to obtain release, extract, `./pioneer --help`, three commands to run examples. Include tested platform limitations.
3. **Build from source:** actual C++ compiler/version, CMake version, configure/build/run/test commands confirmed in a clean environment.
4. **CLI usage:** `schedule`, `knapsack`, `cleanup`; `--input` or stdin, `--machine`, `--help`, errors.
5. **Input/output formats:** exact grammar for each task, provided sample fixture and sample result for each. Explain time/cent restrictions for A and original indices for C.
6. **Algorithms at a glance:** DP used and big-O; link PDF for details, not duplicate it all.
7. **Testing:** how to execute CTest and where actual named test results live.
8. **Project organization:** concise source tree summary.
9. **Development/attribution:** actual original human developer and Part III human contributions, accurate Claude/Kiro/other AI tool roles, independently performed verification.
10. **Known limitations and assumptions:** actual supported platforms, numeric/resource caps.

Keep it concise and task-oriented, following earlier Pioneer Delivery styling when genuinely available. **Update all sample commands and outputs against the implementation; do not leave phantom commands.**
