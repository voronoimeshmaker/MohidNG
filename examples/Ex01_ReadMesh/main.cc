#include <filesystem>
#include <iostream>

#include "MohidNG/Core/Version.h"
#include "MohidNG/IO/MeshReader.h"

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "Usage: Ex01_ReadMesh <mesh-file>\n";
    return 1;
  }

  const auto mesh = mohidng::ReadBootstrapMesh(std::filesystem::path(argv[1]));
  std::cout << mohidng::ProjectName() << " " << mohidng::Version() << "\n";
  std::cout << "Cells: " << mesh.Cells().size() << "\n";
  std::cout << "Faces: " << mesh.Faces().size() << "\n";
  std::cout << "Nodes: " << mesh.Nodes().size() << "\n";
  std::cout << "Boundary patches: " << mesh.BoundaryPatches().size() << "\n";
  return 0;
}
