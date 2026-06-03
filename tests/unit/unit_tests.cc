#include <cassert>
#include <cmath>
#include <filesystem>

#include "MohidNG/Fields/Field.h"
#include "MohidNG/IO/MeshReader.h"
#include "MohidNG/Numerics/Gradient.h"

int main(int argc, char** argv) {
  assert(argc == 2);
  const auto mesh = mohidng::ReadBootstrapMesh(std::filesystem::path(argv[1]));
  assert(mesh.Cells().size() == 4);
  assert(mesh.Faces().size() == 12);
  assert(mesh.BoundaryPatches().size() == 4);

  mohidng::ScalarField constant("constant", mohidng::FieldLocation::kCell, mesh.Cells().size());
  for (std::size_t i = 0; i < constant.Size(); ++i) {
    constant[i] = 3.0;
  }
  const auto grad_constant = mohidng::ComputeCellGradient(mesh, constant);
  for (std::size_t i = 0; i < grad_constant.Size(); ++i) {
    assert(std::abs(grad_constant[i].x) < 1.0e-12);
    assert(std::abs(grad_constant[i].y) < 1.0e-12);
  }
  return 0;
}
