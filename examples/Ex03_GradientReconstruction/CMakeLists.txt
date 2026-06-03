add_executable(Ex03_GradientReconstruction main.cc)
target_link_libraries(Ex03_GradientReconstruction PRIVATE MohidNG::MohidNG)
add_test(NAME Ex03_GradientReconstruction COMMAND Ex03_GradientReconstruction ${CMAKE_SOURCE_DIR}/data/meshes/square_2x2.mngmesh)
