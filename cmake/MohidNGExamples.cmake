# SPDX-License-Identifier: MPL-2.0
include("${CMAKE_CURRENT_LIST_DIR}/testing/MohidNGPrograms.cmake")

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
  "${PROJECT_SOURCE_DIR}/examples/exe_*.cc"
  "${PROJECT_SOURCE_DIR}/examples/exe_*.cpp")
list(SORT MOHIDNG_EXAMPLE_SOURCES)

if(NOT MOHIDNG_EXAMPLE_SOURCES)
  message(STATUS "[MohidNG examples] No exe_*.cc or exe_*.cpp files found")
  return()
endif()

add_custom_target(examples)
add_custom_target(run_all_examples)

foreach(example_source IN LISTS MOHIDNG_EXAMPLE_SOURCES)
  get_filename_component(example_target "${example_source}" NAME_WE)
  set(example_libraries "")
  set(example_include_directories "")
  if(example_target STREQUAL "exe_rectangular_domain_gradient")
    if(TARGET yaml-cpp::yaml-cpp)
      list(APPEND example_libraries yaml-cpp::yaml-cpp)
    elseif(TARGET yaml-cpp)
      list(APPEND example_libraries yaml-cpp)
    else()
      list(APPEND example_include_directories ${YAML_CPP_INCLUDE_DIRS})
      list(APPEND example_libraries ${YAML_CPP_LIBRARIES})
    endif()
  endif()

  set(example_input "")
  if(example_target STREQUAL "exe_read_mesh" OR example_target STREQUAL "exe_cell_field"
      OR example_target STREQUAL "exe_gradient_reconstruction")
    set(example_input "${MOHIDNG_BOOTSTRAP_MESH}")
  elseif(example_target STREQUAL "exe_read_voronoi_package")
    set(example_input "${MOHIDNG_BOOTSTRAP_PACKAGE}")
  elseif(example_target STREQUAL "exe_rectangular_domain_gradient")
    set(example_input "${MOHIDNG_RECTANGULAR_GRADIENT_CASE}")
  endif()

  mohidng_add_program("${example_source}" examples
    ARGUMENTS ${example_input}
    LIBRARIES ${example_libraries}
    INCLUDE_DIRECTORIES ${example_include_directories})
endforeach()
