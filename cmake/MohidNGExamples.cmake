set(MOHIDNG_BOOTSTRAP_MESH
  ${PROJECT_SOURCE_DIR}/data/meshes/square_2x2.mngmesh
  CACHE FILEPATH "Bootstrap mesh used by example and test run targets")
set(MOHIDNG_BOOTSTRAP_PACKAGE
  ${PROJECT_SOURCE_DIR}/data/meshes/square_2x2.mngpkg.h5
  CACHE FILEPATH "Bootstrap Voronoi mesh package used by package-reader examples")
set(MOHIDNG_RECTANGULAR_GRADIENT_CASE
  ${PROJECT_SOURCE_DIR}/examples/Ex05_RectangularDomainGradient/RectangularDomainGradient.ini
  CACHE FILEPATH "Input file used by the rectangular gradient example")

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
  if(example_target STREQUAL "exe_rectangular_domain_gradient")
    if(TARGET yaml-cpp::yaml-cpp)
      target_link_libraries("${example_target}" PRIVATE yaml-cpp::yaml-cpp)
    elseif(TARGET yaml-cpp)
      target_link_libraries("${example_target}" PRIVATE yaml-cpp)
    else()
      target_include_directories("${example_target}" PRIVATE ${YAML_CPP_INCLUDE_DIRS})
      target_link_libraries("${example_target}" PRIVATE ${YAML_CPP_LIBRARIES})
    endif()
  endif()

  set(example_input "${MOHIDNG_BOOTSTRAP_MESH}")
  set(example_has_input TRUE)
  if(example_target STREQUAL "exe_read_voronoi_package")
    set(example_input "${MOHIDNG_BOOTSTRAP_PACKAGE}")
  elseif(example_target STREQUAL "exe_rectangular_domain_gradient")
    set(example_input "${MOHIDNG_RECTANGULAR_GRADIENT_CASE}")
  endif()

  add_dependencies(examples "${example_target}")
  if(example_has_input)
    add_test(NAME "${example_target}"
      COMMAND "${example_target}" "${example_input}")

    add_custom_target("run_${example_run_name}"
      COMMAND "${example_target}" "${example_input}"
      DEPENDS "${example_target}"
      WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
      COMMENT "Running ${example_target}")
  else()
    add_test(NAME "${example_target}"
      COMMAND "${example_target}")

    add_custom_target("run_${example_run_name}"
      COMMAND "${example_target}"
      DEPENDS "${example_target}"
      WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
      COMMENT "Running ${example_target}")
  endif()
  add_dependencies(run_all_examples "run_${example_run_name}")
endforeach()
