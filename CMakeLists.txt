cmake_minimum_required(VERSION 3.22)

project(MohidNG
  VERSION 0.0.1
  DESCRIPTION "Voronoi-based environmental modelling core"
  LANGUAGES CXX)

option(MOHIDNG_BUILD_EXAMPLES "Build Mohid-NG examples" ON)
option(MOHIDNG_BUILD_TESTS "Build Mohid-NG tests" ON)
option(MOHIDNG_BUILD_DOCS "Build Mohid-NG documentation" OFF)

include(cmake/MohidNGCompilerOptions.cmake)

add_library(MohidNG STATIC
  src/Core/Error.cc
  src/Core/Version.cc
  src/Mesh/MeshView.cc
  src/Fields/FieldSet.cc
  src/Numerics/Gradient.cc
  src/IO/MeshReader.cc)

add_library(MohidNG::MohidNG ALIAS MohidNG)

target_include_directories(MohidNG
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)

target_compile_features(MohidNG PUBLIC cxx_std_20)

target_link_libraries(MohidNG PUBLIC MohidNG::CompilerOptions)

if(MOHIDNG_BUILD_TESTS OR MOHIDNG_BUILD_EXAMPLES)
  enable_testing()
endif()

if(MOHIDNG_BUILD_EXAMPLES)
  add_subdirectory(examples/Ex01_ReadMesh)
  add_subdirectory(examples/Ex02_CellField)
  add_subdirectory(examples/Ex03_GradientReconstruction)
endif()

if(MOHIDNG_BUILD_TESTS)
  add_subdirectory(tests/unit)
  add_subdirectory(tests/integration)
endif()
