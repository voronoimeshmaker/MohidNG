add_library(MohidNGCompilerOptions INTERFACE)
add_library(MohidNG::CompilerOptions ALIAS MohidNGCompilerOptions)

target_compile_features(MohidNGCompilerOptions INTERFACE cxx_std_20)

if(MSVC)
  target_compile_options(MohidNGCompilerOptions INTERFACE /W4 /permissive-)
else()
  target_compile_options(MohidNGCompilerOptions INTERFACE
    -Wall
    -Wextra
    -Wpedantic
    -Wconversion
    -Wshadow
    -Wnon-virtual-dtor)
endif()
