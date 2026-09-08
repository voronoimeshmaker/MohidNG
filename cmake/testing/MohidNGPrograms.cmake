# SPDX-License-Identifier: MPL-2.0
include_guard(GLOBAL)

function(mohidng_add_program source group)
  cmake_parse_arguments(PARSE_ARGV 2 PROGRAM "GTEST;NO_CTEST" "" "ARGUMENTS;LIBRARIES;INCLUDE_DIRECTORIES")
  if(PROGRAM_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "Unknown MohidNG program options: ${PROGRAM_UNPARSED_ARGUMENTS}")
  endif()
  get_filename_component(target "${source}" NAME_WE)
  if(TARGET "${target}")
    get_target_property(previous_source "${target}" MOHIDNG_PROGRAM_SOURCE)
    message(FATAL_ERROR "Duplicate MohidNG program '${target}': ${previous_source}; ${source}")
  endif()

  string(TOUPPER "${target}" key)
  set(argument_option "MOHIDNG_${key}_ARGUMENTS")
  set(library_option "MOHIDNG_${key}_LIBRARIES")
  set(directory_option "MOHIDNG_${key}_WORKING_DIRECTORY")
  # Only explicit per-program overrides are cached; shared input defaults can change.
  set(program_arguments "${PROGRAM_ARGUMENTS}")
  if(DEFINED ${argument_option})
    set(program_arguments "${${argument_option}}")
  endif()
  set(program_libraries "${PROGRAM_LIBRARIES}")
  if(DEFINED ${library_option})
    set(program_libraries "${${library_option}}")
  endif()
  set(default_directory "${CMAKE_CURRENT_BINARY_DIR}/runs/${target}")
  if(CMAKE_CONFIGURATION_TYPES)
    set(default_directory "${CMAKE_CURRENT_BINARY_DIR}/runs/$<CONFIG>/${target}")
  endif()
  set("${directory_option}" "${default_directory}" CACHE PATH "Working directory for ${target}")
  if(CMAKE_CONFIGURATION_TYPES)
    foreach(configuration IN LISTS CMAKE_CONFIGURATION_TYPES)
      string(REPLACE "$<CONFIG>" "${configuration}" working_directory "${${directory_option}}")
      file(MAKE_DIRECTORY "${working_directory}")
    endforeach()
  else()
    file(MAKE_DIRECTORY "${${directory_option}}")
  endif()

  add_executable("${target}" "${source}")
  set_property(TARGET "${target}" PROPERTY MOHIDNG_PROGRAM_SOURCE "${source}")
  string(FIND "${target}" "_" separator)
  math(EXPR name_start "${separator} + 1")
  string(SUBSTRING "${target}" ${name_start} -1 short_name)
  set_property(TARGET "${target}" PROPERTY MOHIDNG_SHORT_RUN_NAME "${short_name}")
  target_link_libraries("${target}" PRIVATE MohidNG::MohidNG ${program_libraries})
  if(PROGRAM_GTEST)
    if(NOT TARGET GTest::gtest_main)
      find_package(GTest REQUIRED)
    endif()
    target_link_libraries("${target}" PRIVATE GTest::gtest_main)
  endif()
  if(PROGRAM_INCLUDE_DIRECTORIES)
    target_include_directories("${target}" PRIVATE ${PROGRAM_INCLUDE_DIRECTORIES})
  endif()
  mohidng_apply_target_settings("${target}")

  if(PROGRAM_GTEST)
    include(GoogleTest)
    gtest_discover_tests("${target}"
      TEST_PREFIX "${target}."
      EXTRA_ARGS ${program_arguments}
      WORKING_DIRECTORY "${${directory_option}}"
      DISCOVERY_MODE PRE_TEST
      PROPERTIES LABELS "${group}")
  elseif(NOT PROGRAM_NO_CTEST)
    add_test(NAME "${target}" COMMAND "$<TARGET_FILE:${target}>" ${program_arguments})
    set_tests_properties("${target}" PROPERTIES
      WORKING_DIRECTORY "${${directory_option}}"
      LABELS "${group}")
  endif()

  add_custom_target("run_${target}"
    COMMAND "$<TARGET_FILE:${target}>" ${program_arguments}
    DEPENDS "${target}"
    WORKING_DIRECTORY "${${directory_option}}"
    COMMENT "Running ${target}"
    VERBATIM)
  add_dependencies("${group}" "${target}")
  add_dependencies("run_all_${group}" "run_${target}")
  set_property(GLOBAL APPEND PROPERTY MOHIDNG_PROGRAM_TARGETS "${target}")
endfunction()

function(mohidng_add_short_runners)
  get_property(programs GLOBAL PROPERTY MOHIDNG_PROGRAM_TARGETS)
  set(short_names "")
  foreach(target IN LISTS programs)
    get_target_property(short_name "${target}" MOHIDNG_SHORT_RUN_NAME)
    list(APPEND short_names "${short_name}")
  endforeach()
  foreach(target IN LISTS programs)
    get_target_property(short_name "${target}" MOHIDNG_SHORT_RUN_NAME)
    set(matches 0)
    foreach(candidate IN LISTS short_names)
      if(candidate STREQUAL short_name)
        math(EXPR matches "${matches} + 1")
      endif()
    endforeach()
    if(matches EQUAL 1 AND NOT TARGET "run_${short_name}")
      add_custom_target("run_${short_name}" DEPENDS "run_${target}")
    else()
      message(STATUS "Short runner run_${short_name} is ambiguous/reserved; use run_${target}")
    endif()
  endforeach()
endfunction()

# Both example and test names must be known before creating short aliases.
cmake_language(DEFER CALL mohidng_add_short_runners)
