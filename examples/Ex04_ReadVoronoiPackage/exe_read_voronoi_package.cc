// =============================================================================
// Example: read a Voronoi mesh package in HDF5 format.
//
// Purpose:
// Exercises the Mohid-NG HDF5 mesh-package reader introduced in Block 1.
// The program opens a Voronoi mesh package, validates the package contract
// through the reader, and prints metadata plus mesh entity counts.
//
// Input:
// A .h5 Voronoi mesh package passed as the first command-line argument.
//
// How to run from the build directory:
// make run_read_voronoi_package
//
// Direct executable form:
// ./exe_read_voronoi_package <mesh-package.h5>
//
// Expected result:
// The program prints schema metadata, producer information, and the number
// of cells, faces, nodes, and boundary patches in the package.
// =============================================================================

#include <filesystem>
#include <iostream>

#include <MohidNG/Core/Error.h>
#include <MohidNG/IO/VoronoiMeshPackageReader.h>


int main(int argc, char** argv) {
  try {
    mohidng::Require(argc == 2, "cli.invalid_arguments",
                     "Usage: exe_read_voronoi_package <mesh-package.h5>");

    const auto mesh = mohidng::ReadVoronoiMeshPackage(std::filesystem::path(argv[1]));
    std::cout << "Schema: " << mesh.Metadata().schema_version << "\n";
    std::cout << "Producer: " << mesh.Metadata().producer << "\n";
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
