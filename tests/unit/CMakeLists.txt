add_executable(UnitTests unit_tests.cc)
target_link_libraries(UnitTests PRIVATE MohidNG::MohidNG)
add_test(NAME UnitTests COMMAND UnitTests ${CMAKE_SOURCE_DIR}/data/meshes/square_2x2.mngmesh)
