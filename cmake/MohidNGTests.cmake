set(MOHIDNG_BOOTSTRAP_MESH
  ${PROJECT_SOURCE_DIR}/data/meshes/square_2x2.mngmesh
  CACHE FILEPATH "Bootstrap mesh used by example and test run targets")

file(GLOB_RECURSE MOHIDNG_TEST_SOURCES
  CONFIGURE_DEPENDS
  ${PROJECT_SOURCE_DIR}/tests/tst_*.cc)
list(SORT MOHIDNG_TEST_SOURCES)

if(NOT MOHIDNG_TEST_SOURCES)
  message(STATUS "[MohidNG tests] No tst_*.cc files found")
  return()
endif()

add_custom_target(tests)
add_custom_target(run_all_tests)

foreach(test_source IN LISTS MOHIDNG_TEST_SOURCES)
  get_filename_component(test_target "${test_source}" NAME_WE)
  string(REGEX REPLACE "^tst_" "" test_run_name "${test_target}")

  add_executable("${test_target}" "${test_source}")
  target_link_libraries("${test_target}" PRIVATE MohidNG::MohidNG)

  add_test(NAME "${test_target}"
    COMMAND "${test_target}" "${MOHIDNG_BOOTSTRAP_MESH}")
  add_dependencies(tests "${test_target}")

  add_custom_target("run_${test_run_name}"
    COMMAND "${test_target}" "${MOHIDNG_BOOTSTRAP_MESH}"
    DEPENDS "${test_target}"
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    COMMENT "Running ${test_target}")
  add_dependencies(run_all_tests "run_${test_run_name}")
endforeach()
