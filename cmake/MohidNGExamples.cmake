set(MOHIDNG_BOOTSTRAP_MESH
  ${PROJECT_SOURCE_DIR}/data/meshes/square_2x2.mngmesh
  CACHE FILEPATH "Bootstrap mesh used by example and test run targets")
set(MOHIDNG_BOOTSTRAP_PACKAGE
  ${PROJECT_SOURCE_DIR}/data/meshes/square_2x2.mngpkg.h5
  CACHE FILEPATH "Bootstrap Voronoi mesh package used by package-reader examples")

file(GLOB_RECURSE MOHIDNG_EXAMPLE_SOURCES
  CONFIGURE_DEPENDS
  ${PROJECT_SOURCE_DIR}/examples/exe_*.cc)
list(SORT MOHIDNG_EXAMPLE_SOURCES)

if(NOT MOHIDNG_EXAMPLE_SOURCES)
  message(STATUS "[MohidNG examples] No exe_*.cc files found")
  return()
endif()

add_custom_target(examples)
add_custom_target(run_all_examples)

foreach(example_source IN LISTS MOHIDNG_EXAMPLE_SOURCES)
  get_filename_component(example_target "${example_source}" NAME_WE)
  string(REGEX REPLACE "^exe_" "" example_run_name "${example_target}")

  add_executable("${example_target}" "${example_source}")
  target_link_libraries("${example_target}" PRIVATE MohidNG::MohidNG)

  set(example_input "${MOHIDNG_BOOTSTRAP_MESH}")
  if(example_target STREQUAL "exe_read_voronoi_package")
    set(example_input "${MOHIDNG_BOOTSTRAP_PACKAGE}")
  endif()

  add_dependencies(examples "${example_target}")
  add_test(NAME "${example_target}"
    COMMAND "${example_target}" "${example_input}")

  add_custom_target("run_${example_run_name}"
    COMMAND "${example_target}" "${example_input}"
    DEPENDS "${example_target}"
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    COMMENT "Running ${example_target}")
  add_dependencies(run_all_examples "run_${example_run_name}")
endforeach()
