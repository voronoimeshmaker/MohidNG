cmake_minimum_required(VERSION 3.22)

project(MohidNG
  VERSION 0.0.1
  DESCRIPTION "Voronoi-based environmental modelling core"
  LANGUAGES CXX)

option(MOHIDNG_BUILD_EXAMPLES "Build Mohid-NG examples" ON)
option(MOHIDNG_BUILD_TESTS "Build Mohid-NG tests" ON)
option(MOHIDNG_BUILD_DOCS "Build Mohid-NG documentation" OFF)

include(cmake/MohidNGCompilerOptions.cmake)

file(GLOB_RECURSE MOHIDNG_LIBRARY_SOURCES
  CONFIGURE_DEPENDS
  ${PROJECT_SOURCE_DIR}/src/*.cc)
list(SORT MOHIDNG_LIBRARY_SOURCES)

add_library(MohidNG STATIC ${MOHIDNG_LIBRARY_SOURCES})

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
  include(cmake/MohidNGExamples.cmake)
endif()

if(MOHIDNG_BUILD_TESTS)
  include(cmake/MohidNGTests.cmake)
endif()
