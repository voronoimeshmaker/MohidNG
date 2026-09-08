# SPDX-License-Identifier: MPL-2.0
cmake_minimum_required(VERSION 3.22)

foreach(required IN ITEMS MOHIDNG_MODULE_DIR MOHIDNG_CHECK_BINARY_DIR MOHIDNG_CHECK_GENERATOR MOHIDNG_CHECK_COMPILER)
  if(NOT DEFINED ${required} OR "${${required}}" STREQUAL "")
    message(FATAL_ERROR "Missing check parameter: ${required}")
  endif()
endforeach()

# Each run owns a fresh generated fixture; no source checkout is edited or cleaned.
string(RANDOM LENGTH 12 ALPHABET 0123456789abcdef probe_id)
set(source "${MOHIDNG_CHECK_BINARY_DIR}/probe-${probe_id}/source")
set(binary "${MOHIDNG_CHECK_BINARY_DIR}/probe-${probe_id}/build")
file(MAKE_DIRECTORY "${source}/examples" "${source}/tests" "${source}/benchmarks" "${source}/cases")

set(fixture_project [=[
cmake_minimum_required(VERSION 3.22)
project(MohidNGBuildProbe LANGUAGES CXX)
option(MOHIDNG_BUILD_EXAMPLES "Build examples" ON)
option(MOHIDNG_BUILD_TESTS "Build tests" ON)
include("@MOHIDNG_MODULE_DIR@/MohidNGCompilerOptions.cmake")
add_library(MohidNG STATIC dummy.cc)
add_library(MohidNG::MohidNG ALIAS MohidNG)
target_link_libraries(MohidNG PUBLIC MohidNG::CompilerOptions)
enable_testing()
if(MOHIDNG_BUILD_EXAMPLES)
  include("@MOHIDNG_MODULE_DIR@/MohidNGExamples.cmake")
endif()
if(MOHIDNG_BUILD_TESTS)
  include("@MOHIDNG_MODULE_DIR@/MohidNGTests.cmake")
endif()
function(verify_settings)
  get_target_property(applied MohidNG MOHIDNG_SETTINGS_APPLIED)
  get_target_property(ipo MohidNG INTERPROCEDURAL_OPTIMIZATION_RELEASE)
  get_target_property(public_flags MohidNGCompilerOptions INTERFACE_COMPILE_OPTIONS)
  if(NOT applied OR ipo OR public_flags)
    message(FATAL_ERROR "Deferred/private target settings were not respected")
  endif()
endfunction()
cmake_language(DEFER CALL verify_settings)
]=])
string(CONFIGURE "${fixture_project}" configured_project @ONLY)
file(WRITE "${source}/CMakeLists.txt" "${configured_project}")
file(WRITE "${source}/dummy.cc" "int mohidng_build_probe() { return 0; }\n")
set(no_arguments "int main(int argc, char**) { return argc == 1 ? 0 : 1; }\n")
file(WRITE "${source}/examples/exe_shared.cpp" "${no_arguments}")
file(WRITE "${source}/examples/exe_multiple_words.cpp" "${no_arguments}")
file(WRITE "${source}/tests/test_shared.cpp" "#include <gtest/gtest.h>\n\nTEST(BuildProbe, GTestDiscovery) { EXPECT_EQ(2 + 2, 4); }\n")
file(WRITE "${source}/examples/exe_read_mesh.cc" "int main(int argc, char**) { return argc == 2 ? 0 : 1; }\n")
file(WRITE "${source}/benchmarks/bench_reference.cpp" "${no_arguments}")
file(WRITE "${source}/cases/case_reference.cpp" "${no_arguments}")

function(require_success context)
  if(NOT result STREQUAL "0")
    message(FATAL_ERROR "${context} failed (${result}):\n${output}\n${error}")
  endif()
endfunction()

function(run_checked)
  execute_process(COMMAND ${ARGV}
    RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE error)
  require_success("${ARGV}")
endfunction()

run_checked("${CMAKE_COMMAND}" -S "${source}" -B "${binary}"
  -G "${MOHIDNG_CHECK_GENERATOR}"
  "-DCMAKE_CXX_COMPILER=${MOHIDNG_CHECK_COMPILER}"
  -DCMAKE_BUILD_TYPE=Release -DMOHIDNG_ENABLE_LTO=OFF
  -DMOHIDNG_BUILD_BENCHMARKS=ON -DMOHIDNG_BUILD_CASES=ON)
run_checked("${CMAKE_COMMAND}" --build "${binary}" --config Release
  --target run_exe_shared run_test_shared run_multiple_words run_read_mesh run_bench_reference run_case_reference --parallel 2)
run_checked("${CMAKE_CTEST_COMMAND}" --test-dir "${binary}" -C Release --output-on-failure)
execute_process(COMMAND "${CMAKE_CTEST_COMMAND}" --test-dir "${binary}" -C Release -N
  RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE error)
require_success("CTest GTest listing")
if(NOT output MATCHES "test_shared.BuildProbe.GTestDiscovery" OR output MATCHES "(bench|case)_reference")
  message(FATAL_ERROR "GTest discovery or scientific CTest opt-in was not respected: ${output}")
endif()

execute_process(COMMAND "${CMAKE_COMMAND}" --build "${binary}" --config Release --target run_shared
  RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE error)
if(result STREQUAL "0")
  message(FATAL_ERROR "An ambiguous short runner was created")
endif()

# A normal incremental build must discover a new folder and both source extensions.
file(MAKE_DIRECTORY "${source}/examples/added")
file(WRITE "${source}/examples/added/exe_added.cc" "${no_arguments}")
file(WRITE "${source}/examples/added/exe_arguments.cpp" "int main(int argc, char**) { return argc == 3 ? 0 : 1; }\n")
run_checked("${CMAKE_COMMAND}" --build "${binary}" --config Release --target examples --parallel 2)
run_checked("${CMAKE_COMMAND}" --build "${binary}" --config Release --target run_added)

execute_process(COMMAND "${CMAKE_COMMAND}" -S "${source}" -B "${binary}"
  "-DMOHIDNG_EXE_ARGUMENTS_ARGUMENTS=argument with spaces;second"
  "-DMOHIDNG_BOOTSTRAP_MESH=${source}/replacement mesh.mngmesh"
  RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE error)
require_success("Per-program arguments configuration")
run_checked("${CMAKE_COMMAND}" --build "${binary}" --config Release --target run_arguments)
file(READ "${binary}/CTestTestfile.cmake" registered_tests)
string(FIND "${registered_tests}" "replacement mesh.mngmesh" updated_input)
if(updated_input EQUAL -1)
  message(FATAL_ERROR "Shared input defaults became stale after reconfiguration")
endif()

file(REMOVE "${source}/examples/added/exe_added.cc")
run_checked("${CMAKE_COMMAND}" --build "${binary}" --config Release --target examples --parallel 2)
file(READ "${binary}/CTestTestfile.cmake" registered_tests)
string(FIND "${registered_tests}" "exe_added" removed_test)
if(NOT removed_test EQUAL -1)
  message(FATAL_ERROR "Removed source is still registered as a test")
endif()

run_checked("${CMAKE_COMMAND}" -S "${source}" -B "${binary}-scientific"
  -G "${MOHIDNG_CHECK_GENERATOR}" "-DCMAKE_CXX_COMPILER=${MOHIDNG_CHECK_COMPILER}"
  -DCMAKE_BUILD_TYPE=Release -DMOHIDNG_ENABLE_LTO=OFF
  -DMOHIDNG_BUILD_EXAMPLES=OFF -DMOHIDNG_BUILD_TESTS=OFF
  -DMOHIDNG_BUILD_BENCHMARKS=ON -DMOHIDNG_BUILD_CASES=ON
  -DMOHIDNG_REGISTER_SCIENTIFIC_TESTS=ON)
run_checked("${CMAKE_COMMAND}" --build "${binary}-scientific" --config Release
  --target run_bench_reference run_case_reference --parallel 2)
run_checked("${CMAKE_CTEST_COMMAND}" --test-dir "${binary}-scientific" -C Release --output-on-failure)

file(MAKE_DIRECTORY "${source}/examples/duplicate")
file(WRITE "${source}/examples/duplicate/exe_shared.cc" "${no_arguments}")
execute_process(COMMAND "${CMAKE_COMMAND}" -S "${source}" -B "${binary}"
  RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE error)
if(result STREQUAL "0" OR NOT "${output}\n${error}" MATCHES "Duplicate MohidNG program")
  message(FATAL_ERROR "Duplicate program names were not diagnosed clearly")
endif()

message(STATUS "MohidNG CMake checks passed: four groups, GTest, discovery, removal, arguments, defaults, aliases, duplicates and private settings")
