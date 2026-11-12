# 14 — Integration, Release, and Canvas ZIP Playbook

## Preconditions

All A/B/C production tickets, W5 verification, D1-D4 evidence, and CLI UX are complete. G0–G4 PASS with recorded evidence. G5 and G5-U still need validation prior to promotion. All workers inactive/cleanly checkpointed, no pending unauthorized files.

## Integration sequence (orchestrator only)

1. Review each worker diff for allowed file scope, security issues, readability, specification compliance and evidence.
2. Commit approved scoped work to the assigned **feature** branch; record ticket ID, reviewer and hash.
3. Merge accepted commits one branch/ticket at a time into orchestrator-owned `integration`, preserving useful history.
4. Run targeted CTest and the full clean CMake/CTest cycle; investigate conflicts and return targeted remediation, not ad-hoc rewrite of everything.
5. Run CLI acceptance for all three official examples (file+stdin, human+machine), help and invalid inputs.
6. Perform G3 independent oracles on the merged code, not only a worker's private snapshot.
7. Have W5 finalize `README.md`, actual `docs/TestResults.md`, and `docs/AlgorithmReport.tex`; compile PDF and visually inspect every page.
8. Test prebuilt Linux quick-start archive on a clean supported environment, measure G5-U.

## Build and report commands (after implementation)

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
ctest --test-dir build --output-on-failure
./build/pioneer --help
./build/pioneer schedule --input examples/schedule.txt
./build/pioneer knapsack --input examples/knapsack.txt
./build/pioneer cleanup --input examples/cleanup.txt
latexmk -pdf -interaction=nonstopmode -halt-on-error -output-directory=docs docs/AlgorithmReport.tex
```

The report command may need adjustment for relative LaTeX assets; use the **actual tested command** in developer docs. The main product CMake build must **not require LaTeX** just to compile the C++ program.

## Final source submission ZIP

Construct from an **allowlist** in a clean export of the verified release commit, rather than zipping a working directory with caches. Include `CMakeLists.txt`, all source/headers, `README.md`, tests/oracles/fixtures, `examples/`, `docs/AlgorithmReport.pdf`, `docs/AlgorithmReport.tex`, `docs/TestResults.md`, relevant license/readme assets if present. Internal tickets/gate logs may be excluded if not needed for reproducibility; the professor's assignment PDF is not required in the submission. Never accidentally exclude test fixtures referred to by CTest.

Archive has **one top-level project folder**, descriptive archive name e.g. `PioneerDelivery-PartIII-Submission.zip`, paths with normal separators and no absolute symlinks. Do not include build output, secrets, `.orchestration/`, `.git`, container images, session databases or stale PDFs.

## Clean-room verification of ZIP

1. Extract ZIP into a fresh directory without relying on Git checkout.
2. Check required file manifest, SHA-256 archive checksum and report PDF existence/legibility.
3. Configure/build/test using only the README steps and declared dependencies.
4. Exercise all three examples from the extracted source; check objective values and reconstructed validity.
5. Compare recorded actual outputs in `docs/TestResults.md` to the verified run; fix stale evidence.
6. Confirm README accurately credits human/original developer and the AI tools **actually used**.
7. Check source and ZIP for credentials, private paths or operational files.

## Promotion and approval

After G5/G5-U PASS and owner approval, promote verified `integration` commit to `main` via approved merge, optionally tag it, build immutable archive from that commit, and report file hashes/manifests. **Do not push to a remote or submit to Canvas without explicit human authorization.** A local artifact ready for manual upload is different from a completed submission.

## Final orchestrator report

State commit IDs, completed tickets, exact tests and counts, actual report verification, five-minute trial measured time/platform, archive path/hash, explicit limitations, unresolved risks, and which user authorization is still needed. Never say 'submitted' unless submission was actually performed with permission.
