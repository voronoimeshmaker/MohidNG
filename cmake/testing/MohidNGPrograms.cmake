# SPDX-License-Identifier: MPL-2.0

include_guard(GLOBAL)


# ==============================================================================
# Program creation
# ==============================================================================
#
# Creates one MOHID-NG executable program.
#
# The function is shared by:
#
#   examples
#   tests
#   benchmarks
#   cases
#
# The file passed through "source" is the program driver. Its filename defines
# the executable target name.
#
# Examples:
#
#   examples/Ex01/exe_read_mesh.cpp
#   tests/unit/test_field.cpp
#   benchmarks/gradient/bench_gradient.cpp
#   cases/shallow_water/case_shallow_water.cpp
#
# A program may consist only of its driver file or may use the following
# directory structure:
#
#   <program-directory>/
#   ├── <driver>.cpp
#   ├── include/
#   │   └── ...
#   └── src/
#       ├── ...
#       └── ...
#
# When present:
#
#   <program-directory>/src/
#
# is searched recursively for .cc and .cpp implementation files.
#
# When present:
#
#   <program-directory>/include/
#
# is added as a private include directory of the executable.
#
# This allows simple programs to remain single-file programs while larger
# examples, tests, benchmarks and cases can be split into independent source
# files without requiring a dedicated CMakeLists.txt in every program folder.
#
function(mohidng_add_program source group)

  cmake_parse_arguments(
    PARSE_ARGV 2
    PROGRAM
    "GTEST;NO_CTEST"
    ""
    "ARGUMENTS;LIBRARIES;INCLUDE_DIRECTORIES"
  )

  if(PROGRAM_UNPARSED_ARGUMENTS)
    message(
      FATAL_ERROR
      "Unknown MohidNG program options: ${PROGRAM_UNPARSED_ARGUMENTS}"
    )
  endif()


  # ----------------------------------------------------------------------------
  # Program identity
  # ----------------------------------------------------------------------------

  get_filename_component(target "${source}" NAME_WE)

  if(TARGET "${target}")
    get_target_property(
      previous_source
      "${target}"
      MOHIDNG_PROGRAM_SOURCE
    )

    message(
      FATAL_ERROR
      "Duplicate MohidNG program '${target}': ${previous_source}; ${source}"
    )
  endif()

  get_filename_component(
    program_directory
    "${source}"
    DIRECTORY
  )


  # ----------------------------------------------------------------------------
  # Per-program configuration
  # ----------------------------------------------------------------------------
  #
  # Every program may override:
  #
  #   command-line arguments
  #   additional libraries
  #   working directory
  #
  # Example for bench_gradient:
  #
  #   MOHIDNG_BENCH_GRADIENT_ARGUMENTS
  #   MOHIDNG_BENCH_GRADIENT_LIBRARIES
  #   MOHIDNG_BENCH_GRADIENT_WORKING_DIRECTORY
  #
  # Only explicit per-program overrides are cached. Shared defaults may change
  # between CMake configurations.
  # ----------------------------------------------------------------------------

  string(
    TOUPPER
    "${target}"
    key
  )

  set(
    argument_option
    "MOHIDNG_${key}_ARGUMENTS"
  )

  set(
    library_option
    "MOHIDNG_${key}_LIBRARIES"
  )

  set(
    directory_option
    "MOHIDNG_${key}_WORKING_DIRECTORY"
  )


  set(
    program_arguments
    "${PROGRAM_ARGUMENTS}"
  )

  if(DEFINED ${argument_option})
    set(
      program_arguments
      "${${argument_option}}"
    )
  endif()


  set(
    program_libraries
    "${PROGRAM_LIBRARIES}"
  )

  if(DEFINED ${library_option})
    set(
      program_libraries
      "${${library_option}}"
    )
  endif()


  # ----------------------------------------------------------------------------
  # Working directory
  # ----------------------------------------------------------------------------

  set(
    default_directory
    "${CMAKE_CURRENT_BINARY_DIR}/runs/${target}"
  )

  if(CMAKE_CONFIGURATION_TYPES)
    set(
      default_directory
      "${CMAKE_CURRENT_BINARY_DIR}/runs/$<CONFIG>/${target}"
    )
  endif()

  set(
    "${directory_option}"
    "${default_directory}"
    CACHE PATH
    "Working directory for ${target}"
  )


  if(CMAKE_CONFIGURATION_TYPES)

    foreach(configuration IN LISTS CMAKE_CONFIGURATION_TYPES)

      string(
        REPLACE
        "$<CONFIG>"
        "${configuration}"
        working_directory
        "${${directory_option}}"
      )

      file(
        MAKE_DIRECTORY
        "${working_directory}"
      )

    endforeach()

  else()

    file(
      MAKE_DIRECTORY
      "${${directory_option}}"
    )

  endif()


  # ----------------------------------------------------------------------------
  # Program implementation sources
  # ----------------------------------------------------------------------------
  #
  # The driver passed to this function is always compiled.
  #
  # If a src/ directory exists beside the driver, all .cc and .cpp files below
  # it are also compiled into the same executable.
  #
  # CONFIGURE_DEPENDS causes CMake to monitor the directory so that adding or
  # removing implementation files triggers regeneration of the build system.
  # ----------------------------------------------------------------------------

  set(
    program_sources
    "${source}"
  )

  set(
    program_source_directory
    "${program_directory}/src"
  )

  if(EXISTS "${program_source_directory}")

    file(
      GLOB_RECURSE
      auxiliary_sources
      CONFIGURE_DEPENDS
      "${program_source_directory}/*.cc"
      "${program_source_directory}/*.cpp"
    )

    list(
      SORT
      auxiliary_sources
    )

    list(
      APPEND
      program_sources
      ${auxiliary_sources}
    )

  endif()


  # ----------------------------------------------------------------------------
  # Executable target
  # ----------------------------------------------------------------------------

  add_executable(
    "${target}"
    ${program_sources}
  )

  set_property(
    TARGET "${target}"
    PROPERTY
    MOHIDNG_PROGRAM_SOURCE
    "${source}"
  )

  set_property(
    TARGET "${target}"
    PROPERTY
    MOHIDNG_PROGRAM_DIRECTORY
    "${program_directory}"
  )


  # ----------------------------------------------------------------------------
  # Short runner name
  # ----------------------------------------------------------------------------
  #
  # Program targets use prefixes such as:
  #
  #   exe_
  #   test_
  #   tst_
  #   bench_
  #   case_
  #
  # The prefix is removed to create a convenient short runner when the resulting
  # name is unique.
  #
  # Example:
  #
  #   bench_gradient
  #
  # provides:
  #
  #   run_bench_gradient
  #
  # and, when unambiguous:
  #
  #   run_gradient
  # ----------------------------------------------------------------------------

  string(
    FIND
    "${target}"
    "_"
    separator
  )

  if(separator EQUAL -1)

    set(
      short_name
      "${target}"
    )

  else()

    math(
      EXPR
      name_start
      "${separator} + 1"
    )

    string(
      SUBSTRING
      "${target}"
      ${name_start}
      -1
      short_name
    )

  endif()

  set_property(
    TARGET "${target}"
    PROPERTY
    MOHIDNG_SHORT_RUN_NAME
    "${short_name}"
  )


  # ----------------------------------------------------------------------------
  # Libraries
  # ----------------------------------------------------------------------------

  target_link_libraries(
    "${target}"
    PRIVATE
    MohidNG::MohidNG
    ${program_libraries}
  )


  # ----------------------------------------------------------------------------
  # GoogleTest support
  # ----------------------------------------------------------------------------

  if(PROGRAM_GTEST)

    if(NOT TARGET GTest::gtest_main)
      find_package(
        GTest
        REQUIRED
      )
    endif()

    target_link_libraries(
      "${target}"
      PRIVATE
      GTest::gtest_main
    )

  endif()


  # ----------------------------------------------------------------------------
  # Program include directory
  # ----------------------------------------------------------------------------
  #
  # A local include/ directory is automatically added when it exists.
  #
  # For:
  #
  #   benchmarks/gradient/include/GradientBenchmark.h
  #
  # the benchmark source may therefore simply use:
  #
  #   #include "GradientBenchmark.h"
  #
  # Additional include directories explicitly supplied to mohidng_add_program()
  # are preserved.
  # ----------------------------------------------------------------------------

  set(
    program_include_directory
    "${program_directory}/include"
  )

  if(EXISTS "${program_include_directory}")

    target_include_directories(
      "${target}"
      PRIVATE
      "${program_include_directory}"
    )

  endif()

  if(PROGRAM_INCLUDE_DIRECTORIES)

    target_include_directories(
      "${target}"
      PRIVATE
      ${PROGRAM_INCLUDE_DIRECTORIES}
    )

  endif()


  # ----------------------------------------------------------------------------
  # Common MOHID-NG target settings
  # ----------------------------------------------------------------------------

  mohidng_apply_target_settings(
    "${target}"
  )


  # ----------------------------------------------------------------------------
  # CTest registration
  # ----------------------------------------------------------------------------

  if(PROGRAM_GTEST)

    include(
      GoogleTest
    )

    gtest_discover_tests(
      "${target}"
      TEST_PREFIX
      "${target}."
      EXTRA_ARGS
      ${program_arguments}
      WORKING_DIRECTORY
      "${${directory_option}}"
      DISCOVERY_MODE
      PRE_TEST
      PROPERTIES
      LABELS
      "${group}"
    )

  elseif(NOT PROGRAM_NO_CTEST)

    add_test(
      NAME
      "${target}"
      COMMAND
      "$<TARGET_FILE:${target}>"
      ${program_arguments}
    )

    set_tests_properties(
      "${target}"
      PROPERTIES
      WORKING_DIRECTORY
      "${${directory_option}}"
      LABELS
      "${group}"
    )

  endif()


  # ----------------------------------------------------------------------------
  # Program runner
  # ----------------------------------------------------------------------------
  #
  # Every program receives an explicit runner:
  #
  #   run_<target>
  #
  # Examples:
  #
  #   run_exe_read_mesh
  #   run_test_field
  #   run_bench_gradient
  #   run_case_shallow_water
  # ----------------------------------------------------------------------------

  add_custom_target(
    "run_${target}"

    COMMAND
    "$<TARGET_FILE:${target}>"
    ${program_arguments}

    DEPENDS
    "${target}"

    WORKING_DIRECTORY
    "${${directory_option}}"

    COMMENT
    "Running ${target}"

    VERBATIM
  )


  # ----------------------------------------------------------------------------
  # Group targets
  # ----------------------------------------------------------------------------
  #
  # The calling module creates targets such as:
  #
  #   examples
  #   tests
  #   benchmarks
  #   cases
  #
  # and:
  #
  #   run_all_examples
  #   run_all_tests
  #   run_all_benchmarks
  #   run_all_cases
  # ----------------------------------------------------------------------------

  add_dependencies(
    "${group}"
    "${target}"
  )

  add_dependencies(
    "run_all_${group}"
    "run_${target}"
  )


  # ----------------------------------------------------------------------------
  # Global program registry
  # ----------------------------------------------------------------------------
  #
  # The registry is evaluated later when short runner aliases are created.
  # ----------------------------------------------------------------------------

  set_property(
    GLOBAL
    APPEND
    PROPERTY
    MOHIDNG_PROGRAM_TARGETS
    "${target}"
  )

endfunction()


# ==============================================================================
# Short runner aliases
# ==============================================================================
#
# Creates:
#
#   run_<short-name>
#
# only when that short name uniquely identifies one MOHID-NG program.
#
# For example:
#
#   bench_gradient -> run_gradient
#
# If another group also contains a program whose short name is "gradient", the
# alias is deliberately not created and the full runner must be used.
#
function(mohidng_add_short_runners)

  get_property(
    programs
    GLOBAL
    PROPERTY
    MOHIDNG_PROGRAM_TARGETS
  )

  if(NOT programs)
    return()
  endif()


  set(
    short_names
    ""
  )


  # Collect all candidate short names.

  foreach(target IN LISTS programs)

    get_target_property(
      short_name
      "${target}"
      MOHIDNG_SHORT_RUN_NAME
    )

    list(
      APPEND
      short_names
      "${short_name}"
    )

  endforeach()


  # Create an alias only when the candidate name is unique.

  foreach(target IN LISTS programs)

    get_target_property(
      short_name
      "${target}"
      MOHIDNG_SHORT_RUN_NAME
    )

    set(
      matches
      0
    )

    foreach(candidate IN LISTS short_names)

      if(candidate STREQUAL short_name)

        math(
          EXPR
          matches
          "${matches} + 1"
        )

      endif()

    endforeach()


    if(matches EQUAL 1 AND
       NOT TARGET "run_${short_name}")

      add_custom_target(
        "run_${short_name}"
        DEPENDS
        "run_${target}"
      )

    else()

      message(
        STATUS
        "Short runner run_${short_name} is ambiguous/reserved; use run_${target}"
      )

    endif()

  endforeach()

endfunction()


# ==============================================================================
# Deferred short-runner creation
# ==============================================================================
#
# Short aliases can only be created after all enabled program groups have been
# discovered. Deferring this call ensures that examples, tests, benchmarks and
# cases are all known before uniqueness is evaluated.
#
cmake_language(
  DEFER
  CALL
  mohidng_add_short_runners
)