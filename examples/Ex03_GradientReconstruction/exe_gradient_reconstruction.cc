#include <cmath>
#include <filesystem>
#include <iostream>

#include "MohidNG/Fields/Field.h"
#include "MohidNG/IO/MeshReader.h"
#include "MohidNG/Numerics/Gradient.h"

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "Usage: Ex03_GradientReconstruction <mesh-file>\n";
    return 1;
  }

  const auto mesh = mohidng::ReadBootstrapMesh(std::filesystem::path(argv[1]));
  mohidng::ScalarField phi("phi", mohidng::FieldLocation::kCell, mesh.Cells().size());

  for (const auto& cell : mesh.Cells()) {
    phi[static_cast<std::size_t>(cell.id.value)] = cell.centre.x + 2.0 * cell.centre.y;
  }

  const auto grad_phi = mohidng::ComputeCellGradient(mesh, phi);
  for (std::size_t i = 0; i < grad_phi.Size(); ++i) {
    const auto gradient = grad_phi[i];
    std::cout << "cell " << i << " gradient = (" << gradient.x << ", " << gradient.y << ")\n";
    if (std::abs(gradient.x - 1.0) > 1.0e-12 || std::abs(gradient.y - 2.0) > 1.0e-12) {
      std::cerr << "Gradient reconstruction failed for linear field.\n";
      return 2;
    }
  }

  return 0;
}
