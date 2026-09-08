# SPDX-License-Identifier: MPL-2.0
include_guard(GLOBAL)

function(mohidng_apply_target_settings target)
  if(NOT TARGET "${target}")
    message(FATAL_ERROR "Cannot configure missing MohidNG target: ${target}")
  endif()
  get_target_property(already_configured "${target}" MOHIDNG_SETTINGS_APPLIED)
  if(already_configured)
    return()
  endif()
  set_property(TARGET "${target}" PROPERTY MOHIDNG_SETTINGS_APPLIED TRUE)

  # Keep warnings/CPU choices private; consumers only inherit API requirements.
  if(MOHIDNG_ENABLE_WARNINGS)
    if(MOHIDNG_MSVC_FRONTEND)
      target_compile_options("${target}" PRIVATE /W4 /permissive-)
    elseif(MOHIDNG_GNU_STYLE_FRONTEND)
      target_compile_options("${target}" PRIVATE
        -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Wnon-virtual-dtor)
    endif()
  endif()

  set_target_properties("${target}" PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION FALSE
    INTERPROCEDURAL_OPTIMIZATION_DEBUG FALSE
    INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL FALSE
    INTERPROCEDURAL_OPTIMIZATION_RELEASE "${MOHIDNG_IPO_ACTIVE}"
    INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO "${MOHIDNG_IPO_ACTIVE}")

  if(NOT MOHIDNG_INSTRUMENTED_BUILD)
    if(MOHIDNG_MSVC_FRONTEND)
      target_compile_options("${target}" PRIVATE "$<$<CONFIG:Release,RelWithDebInfo>:/O2>")
    elseif(MOHIDNG_GNU_STYLE_FRONTEND)
      target_compile_options("${target}" PRIVATE "$<$<CONFIG:Release,RelWithDebInfo>:-O3>")
      if(MOHIDNG_ENABLE_NATIVE_ARCH)
        target_compile_options("${target}" PRIVATE "$<$<CONFIG:Release,RelWithDebInfo>:-march=native>")
      endif()
    endif()
  elseif(MOHIDNG_ENABLE_SANITIZERS)
    target_compile_options("${target}" PRIVATE -O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined)
    target_link_options("${target}" PRIVATE -fsanitize=address,undefined)
  elseif(MOHIDNG_ENABLE_COVERAGE)
    target_compile_options("${target}" PRIVATE -O0 -g --coverage)
    target_link_options("${target}" PRIVATE --coverage)
  endif()

  # A build-tree consumer of an instrumented static library needs its runtime.
  get_target_property(target_type "${target}" TYPE)
  if(target_type STREQUAL "STATIC_LIBRARY")
    if(MOHIDNG_ENABLE_SANITIZERS)
      target_link_options("${target}" INTERFACE "$<BUILD_INTERFACE:-fsanitize=address,undefined>")
    elseif(MOHIDNG_ENABLE_COVERAGE)
      target_link_options("${target}" INTERFACE "$<BUILD_INTERFACE:--coverage>")
    endif()
  endif()
endfunction()
