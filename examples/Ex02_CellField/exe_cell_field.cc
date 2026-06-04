#include <algorithm>
#include <filesystem>
#include <iostream>

#include "MohidNG/Fields/Field.h"
#include "MohidNG/IO/MeshReader.h"

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "Usage: Ex02_CellField <mesh-file>\n";
    return 1;
  }

  const auto mesh = mohidng::ReadBootstrapMesh(std::filesystem::path(argv[1]));
  mohidng::ScalarField elevation("surface_elevation", mohidng::FieldLocation::kCell,
                                 mesh.Cells().size());

  for (const auto& cell : mesh.Cells()) {
    elevation[static_cast<std::size_t>(cell.id.value)] = cell.centre.x + 2.0 * cell.centre.y;
  }

  const auto values = elevation.Values();
  const auto [min_it, max_it] = std::minmax_element(values.begin(), values.end());
  std::cout << "Field: " << elevation.Name() << "\n";
  std::cout << "Minimum: " << *min_it << "\n";
  std::cout << "Maximum: " << *max_it << "\n";
  return 0;
}
