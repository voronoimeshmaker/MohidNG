add_executable(Ex01_ReadMesh main.cc)
target_link_libraries(Ex01_ReadMesh PRIVATE MohidNG::MohidNG)
add_test(NAME Ex01_ReadMesh COMMAND Ex01_ReadMesh ${CMAKE_SOURCE_DIR}/data/meshes/square_2x2.mngmesh)
