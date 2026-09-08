# SPDX-License-Identifier: MPL-2.0
include_guard(GLOBAL)

include("${CMAKE_CURRENT_LIST_DIR}/project/MohidNGOptions.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/project/MohidNGTargetSettings.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/testing/MohidNGScientificPrograms.cmake")

add_library(MohidNGCompilerOptions INTERFACE)
add_library(MohidNG::CompilerOptions ALIAS MohidNGCompilerOptions)
target_compile_features(MohidNGCompilerOptions INTERFACE cxx_std_20)

# The unchanged root includes this module before declaring the library.
cmake_language(DEFER CALL mohidng_apply_target_settings MohidNG)
cmake_language(DEFER CALL mohidng_add_scientific_programs)

if(MOHIDNG_REGISTER_SCIENTIFIC_TESTS)
  enable_testing()
endif()
