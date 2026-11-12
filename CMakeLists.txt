cmake_minimum_required(VERSION 3.20)

project(PioneerDelivery VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
  set(pioneerWarnings /W4 /permissive-)
else()
  set(pioneerWarnings -Wall -Wextra -Wpedantic -Wshadow)
endif()

# Parsing, output, and algorithm modules. Nothing here depends on the command line.
add_library(pioneer_core STATIC
  src/Input.cpp
  src/Output.cpp
  src/Scheduling.cpp
  src/Knapsack.cpp
  src/Cleanup.cpp)
target_include_directories(pioneer_core PUBLIC include)
target_compile_features(pioneer_core PUBLIC cxx_std_20)
target_compile_options(pioneer_core PRIVATE ${pioneerWarnings})

add_executable(pioneer src/main.cpp)
target_link_libraries(pioneer PRIVATE pioneer_core)
target_compile_options(pioneer PRIVATE ${pioneerWarnings})

include(CTest)
if(BUILD_TESTING)
  add_executable(parser_tests tests/ParserTests.cpp)
  target_link_libraries(parser_tests PRIVATE pioneer_core)
  target_compile_options(parser_tests PRIVATE ${pioneerWarnings})

  add_executable(formatter_tests tests/FormatterTests.cpp)
  target_link_libraries(formatter_tests PRIVATE pioneer_core)
  target_compile_options(formatter_tests PRIVATE ${pioneerWarnings})

  # Algorithm and oracle tests. Each worker adds its own test file; a target
  # exists only when its file does, so the foundation builds without them.
  foreach(testName IN ITEMS SchedulingTests KnapsackTests CleanupTests)
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/tests/${testName}.cpp)
      add_executable(${testName} tests/${testName}.cpp)
      target_link_libraries(${testName} PRIVATE pioneer_core)
      target_compile_options(${testName} PRIVATE ${pioneerWarnings})
      add_test(NAME ${testName} COMMAND ${testName})
    endif()
  endforeach()
  foreach(testName IN ITEMS SchedulingOracleTests KnapsackOracleTests CleanupOracleTests)
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/tests/oracles/${testName}.cpp)
      add_executable(${testName} tests/oracles/${testName}.cpp)
      target_include_directories(${testName} PRIVATE tests)
      target_link_libraries(${testName} PRIVATE pioneer_core)
      target_compile_options(${testName} PRIVATE ${pioneerWarnings})
      add_test(NAME ${testName} COMMAND ${testName})
    endif()
  endforeach()

  add_test(NAME parser_tests COMMAND parser_tests)
  add_test(NAME formatter_tests COMMAND formatter_tests)
  add_test(NAME cli_checks
    COMMAND ${CMAKE_COMMAND}
            -DPIONEER_EXE=$<TARGET_FILE:pioneer>
            -DWORK_DIR=${CMAKE_CURRENT_BINARY_DIR}
            -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/CliChecks.cmake)
endif()
