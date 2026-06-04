#include <cmath>
#include <filesystem>
#include <stdexcept>
#include <string>

#include "MohidNG/Fields/Field.h"
#include "MohidNG/IO/MeshReader.h"
#include "MohidNG/Numerics/Gradient.h"

namespace {

void Check(bool condition, const std::string& message) {
  if (!condition) {
    throw std::runtime_error(message);
  }
}

}  // namespace

int main(int argc, char** argv) {
  Check(argc == 2, "Expected mesh path argument.");
  const auto mesh = mohidng::ReadBootstrapMesh(std::filesystem::path(argv[1]));
  mohidng::ScalarField linear("linear", mohidng::FieldLocation::kCell, mesh.Cells().size());

  for (const auto& cell : mesh.Cells()) {
    linear[static_cast<std::size_t>(cell.id.value)] = 5.0 + cell.centre.x + 2.0 * cell.centre.y;
  }

  const auto gradient = mohidng::ComputeCellGradient(mesh, linear);
  for (std::size_t i = 0; i < gradient.Size(); ++i) {
    Check(std::abs(gradient[i].x - 1.0) < 1.0e-12, "Unexpected linear x-gradient.");
    Check(std::abs(gradient[i].y - 2.0) < 1.0e-12, "Unexpected linear y-gradient.");
  }
  return 0;
}
