# SPDX-License-Identifier: MPL-2.0
include("${CMAKE_CURRENT_LIST_DIR}/testing/MohidNGPrograms.cmake")

set(MOHIDNG_BOOTSTRAP_MESH
  ${PROJECT_SOURCE_DIR}/data/meshes/square_2x2.mngmesh
  CACHE FILEPATH "Bootstrap mesh used by example and test run targets")
set(MOHIDNG_BOOTSTRAP_PACKAGE
  ${PROJECT_SOURCE_DIR}/data/meshes/square_2x2.mngpkg.h5
  CACHE FILEPATH "Bootstrap Voronoi mesh package used by package-reader tests")

file(GLOB_RECURSE MOHIDNG_TEST_SOURCES
  CONFIGURE_DEPENDS
  "${PROJECT_SOURCE_DIR}/tests/test_*.cpp"
  "${PROJECT_SOURCE_DIR}/tests/test_*.cc"
  "${PROJECT_SOURCE_DIR}/tests/tst_*.cc"
  "${PROJECT_SOURCE_DIR}/tests/tst_*.cpp")
list(SORT MOHIDNG_TEST_SOURCES)

if(NOT MOHIDNG_TEST_SOURCES)
  message(STATUS "[MohidNG tests] No test_* programs or legacy tst_* programs found")
  return()
endif()

add_custom_target(tests)
add_custom_target(run_all_tests)

foreach(test_source IN LISTS MOHIDNG_TEST_SOURCES)
  get_filename_component(test_target "${test_source}" NAME_WE)
  set(test_input "")
  set(legacy_driver OFF)
  if(test_target STREQUAL "tst_voronoi_mesh_package" OR test_target STREQUAL "tst_integration_gradient" OR test_target STREQUAL "tst_gradient_methods")
    set(test_input "${MOHIDNG_BOOTSTRAP_PACKAGE}")
    set(legacy_driver ON)
  elseif(test_target STREQUAL "tst_unit_tests")
    set(test_input "${MOHIDNG_BOOTSTRAP_MESH}")
    set(legacy_driver ON)
  endif()
  if(legacy_driver)
    # These four existing drivers keep their main until a separate C++ migration.
    mohidng_add_program("${test_source}" tests ARGUMENTS ${test_input})
  else()
    mohidng_add_program("${test_source}" tests GTEST)
  endif()
endforeach()

add_custom_target(check_cmake
  COMMAND "${CMAKE_COMMAND}"
    "-DMOHIDNG_MODULE_DIR=${CMAKE_CURRENT_LIST_DIR}"
    "-DMOHIDNG_CHECK_BINARY_DIR=${CMAKE_CURRENT_BINARY_DIR}/cmake-checks"
    "-DMOHIDNG_CHECK_GENERATOR=${CMAKE_GENERATOR}"
    "-DMOHIDNG_CHECK_COMPILER=${CMAKE_CXX_COMPILER}"
    -P "${CMAKE_CURRENT_LIST_DIR}/testing/VerifyInfrastructure.cmake"
  COMMENT "Checking MohidNG CMake discovery and runners"
  VERBATIM)
