add_executable(IntegrationGradient integration_gradient.cc)
target_link_libraries(IntegrationGradient PRIVATE MohidNG::MohidNG)
add_test(NAME IntegrationGradient COMMAND IntegrationGradient ${CMAKE_SOURCE_DIR}/data/meshes/square_2x2.mngmesh)
