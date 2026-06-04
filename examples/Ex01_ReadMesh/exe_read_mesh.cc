// =============================================================================
// Example: read a bootstrap mesh file.
//
// Purpose:
// Demonstrates the first mesh-reading path used in the project bootstrap.
// The program receives a text mesh file, builds a MeshView, and prints the
// number of cells, faces, nodes, and boundary patches found in the mesh.
//
// Input:
// A bootstrap mesh file passed as the first command-line argument.
//
// How to run from the build directory:
// make run_read_mesh
//
// Direct executable form:
// ./exe_read_mesh <mesh-file>
//
// Expected result:
// The program prints the Mohid-NG version and the mesh entity counts.
// =============================================================================

#include <filesystem>
#include <iostream>

#include <MohidNG/Core/Error.h>
#include <MohidNG/Core/Version.h>
#include <MohidNG/IO/MeshReader.h>


int main(int argc, char** argv) {
  try {
    mohidng::Require(argc == 2, "cli.invalid_arguments", "Usage: exe_read_mesh <mesh-file>");

    const auto mesh = mohidng::ReadBootstrapMesh(std::filesystem::path(argv[1]));
    std::cout << mohidng::ProjectName() << " " << mohidng::Version() << "\n";
    std::cout << "Cells: " << mesh.Cells().size() << "\n";
    std::cout << "Faces: " << mesh.Faces().size() << "\n";
    std::cout << "Nodes: " << mesh.Nodes().size() << "\n";
    std::cout << "Boundary patches: " << mesh.BoundaryPatches().size() << "\n";
    return 0;
  } catch (const mohidng::MohidNgException& exception) {
    std::cerr << exception.what() << "\n";
    return 1;
  }
}

