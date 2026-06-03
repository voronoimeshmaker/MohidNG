add_executable(Ex02_CellField main.cc)
target_link_libraries(Ex02_CellField PRIVATE MohidNG::MohidNG)
add_test(NAME Ex02_CellField COMMAND Ex02_CellField ${CMAKE_SOURCE_DIR}/data/meshes/square_2x2.mngmesh)
