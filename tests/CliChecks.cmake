# Black-box checks of the pioneer executable that need no algorithm results:
# help output, usage errors (exit 2), input validation (exit 2), and file errors (exit 1).
# Run as: cmake -DPIONEER_EXE=<path> -DWORK_DIR=<writable dir> -P CliChecks.cmake

if(NOT DEFINED PIONEER_EXE OR NOT DEFINED WORK_DIR)
  message(FATAL_ERROR "CliChecks.cmake needs -DPIONEER_EXE and -DWORK_DIR")
endif()

if(WIN32)
  set(noInput NUL)
else()
  set(noInput /dev/null)
endif()

# Runs pioneer and fails unless its exit status equals expectedCode. stdinFile may be
# empty, which reads from the null device. Captured output is returned in lastStdout
# and lastStderr.
function(runPioneer caseName expectedCode stdinFile)
  set(inputSource "${stdinFile}")
  if(inputSource STREQUAL "")
    set(inputSource "${noInput}")
  endif()
  execute_process(
    COMMAND "${PIONEER_EXE}" ${ARGN}
    INPUT_FILE "${inputSource}"
    OUTPUT_VARIABLE stdoutText
    ERROR_VARIABLE stderrText
    RESULT_VARIABLE exitStatus)
  if(NOT exitStatus STREQUAL "${expectedCode}")
    message(FATAL_ERROR "${caseName}: expected exit status ${expectedCode}, got ${exitStatus}\nstderr:\n${stderrText}")
  endif()
  set(lastStdout "${stdoutText}" PARENT_SCOPE)
  set(lastStderr "${stderrText}" PARENT_SCOPE)
endfunction()

function(requireText caseName haystack needle)
  string(FIND "${haystack}" "${needle}" position)
  if(position EQUAL -1)
    message(FATAL_ERROR "${caseName}: output does not contain '${needle}'\n${haystack}")
  endif()
endfunction()

function(requireEmpty caseName text)
  if(NOT "${text}" STREQUAL "")
    message(FATAL_ERROR "${caseName}: expected empty output, got:\n${text}")
  endif()
endfunction()

file(WRITE "${WORK_DIR}/cli_bad_schedule.txt" "1\nA 5 2 10\n")
file(WRITE "${WORK_DIR}/cli_bad_cleanup.txt" "abbaa\naba\n1\n7\n")
set(missingFile "${WORK_DIR}/cli_no_such_file.txt")
file(REMOVE "${missingFile}")

# Help goes to stdout with exit 0.
runPioneer("no arguments" 0 "")
requireText("no arguments" "${lastStdout}" "Usage:")
requireText("no arguments" "${lastStdout}" "schedule")
requireEmpty("no arguments stderr" "${lastStderr}")

runPioneer("root --help" 0 "" --help)
requireText("root --help" "${lastStdout}" "Commands:")
requireEmpty("root --help stderr" "${lastStderr}")

runPioneer("root -h" 0 "" -h)
requireText("root -h" "${lastStdout}" "Exit status:")

runPioneer("schedule --help" 0 "" schedule --help)
requireText("schedule --help" "${lastStdout}" "Usage: pioneer schedule")

runPioneer("knapsack --help" 0 "" knapsack --help)
requireText("knapsack --help" "${lastStdout}" "Usage: pioneer knapsack")

runPioneer("cleanup --help" 0 "" cleanup -h)
requireText("cleanup --help" "${lastStdout}" "Usage: pioneer cleanup")

# Usage errors: exit 2, usage on stderr, nothing on stdout.
runPioneer("unknown command" 2 "" bogus)
requireEmpty("unknown command stdout" "${lastStdout}")
requireText("unknown command" "${lastStderr}" "unknown command 'bogus'")
requireText("unknown command usage" "${lastStderr}" "usage:")

runPioneer("unknown root option" 2 "" --machine)
requireEmpty("unknown root option stdout" "${lastStdout}")

runPioneer("unknown subcommand option" 2 "" schedule --bogus)
requireEmpty("unknown subcommand option stdout" "${lastStdout}")
requireText("unknown subcommand option" "${lastStderr}" "unknown argument '--bogus'")

runPioneer("--input without value" 2 "" schedule --input)
requireEmpty("--input without value stdout" "${lastStdout}")
requireText("--input without value" "${lastStderr}" "--input needs a PATH")

runPioneer("extra positional argument" 2 "" schedule extra)
requireEmpty("extra positional stdout" "${lastStdout}")
requireText("extra positional" "${lastStderr}" "unknown argument 'extra'")

# Validation failures: exit 2 with the offending record or field named on stderr.
runPioneer("invalid schedule on stdin" 2 "${WORK_DIR}/cli_bad_schedule.txt" schedule)
requireEmpty("invalid schedule stdout" "${lastStdout}")
requireText("invalid schedule stderr" "${lastStderr}" "record 1: finish must be greater than start")

runPioneer("invalid cleanup via --input" 2 "" cleanup --input "${WORK_DIR}/cli_bad_cleanup.txt")
requireEmpty("invalid cleanup stdout" "${lastStdout}")
requireText("invalid cleanup stderr" "${lastStderr}" "target index 7 is out of bounds for source length 5")

# Operational failure: unreadable file gives exit 1.
runPioneer("missing input file" 1 "" knapsack --input "${missingFile}")
requireEmpty("missing input file stdout" "${lastStdout}")
requireText("missing input file stderr" "${lastStderr}" "cannot read input file")

runPioneer("missing input file with --machine" 1 "" schedule --machine --input "${missingFile}")
requireEmpty("missing input file (machine) stdout" "${lastStdout}")

message(STATUS "CLI checks passed")
