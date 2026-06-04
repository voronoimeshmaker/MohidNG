#include <filesystem>
#include <iostream>

#include "MohidNG/IO/VoronoiMeshPackageReader.h"

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "Usage: exe_read_voronoi_package <mesh-package.h5>\n";
    return 1;
  }

  const auto mesh = mohidng::ReadVoronoiMeshPackage(std::filesystem::path(argv[1]));
  std::cout << "Schema: " << mesh.Metadata().schema_version << "\n";
  std::cout << "Producer: " << mesh.Metadata().producer << "\n";
  std::cout << "Cells: " << mesh.Cells().size() << "\n";
  std::cout << "Faces: " << mesh.Faces().size() << "\n";
  std::cout << "Nodes: " << mesh.Nodes().size() << "\n";
  std::cout << "Boundary patches: " << mesh.BoundaryPatches().size() << "\n";
  return 0;
}
