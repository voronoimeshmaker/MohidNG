#include <cmath>
#include <filesystem>
#include <stdexcept>
#include <string>

#include <MohidNG/Core/Constants.h>
#include <MohidNG/Fields/Field.h>
#include <MohidNG/IO/VoronoiMeshPackageReader.h>
#include <MohidNG/Numerics/Gradient/Gradient.h>


namespace {

void Check(bool condition, const std::string& message) {
  if (!condition) {
    throw std::runtime_error(message);
  }
}

mohidng::ScalarField MakeLinearField(const mohidng::MeshView& mesh) {
  mohidng::ScalarField linear("linear", mohidng::CellFieldLocation(), mesh.Cells().size());
  for (const auto& cell : mesh.Cells()) {
    linear[static_cast<std::size_t>(cell.id.value)] = 5.0 + cell.centre.x + 2.0 * cell.centre.y;
  }
  return linear;
}

mohidng::ScalarField MakeConstantField(const mohidng::MeshView& mesh) {
  mohidng::ScalarField constant("constant", mohidng::CellFieldLocation(), mesh.Cells().size());
  for (std::size_t i = 0; i < constant.Size(); ++i) {
    constant[i] = 3.0;
  }
  return constant;
}

void CheckGradient(const mohidng::Vector2Field& gradient, mohidng::Real expected_x, mohidng::Real expected_y,
                   const std::string& label) {
  for (std::size_t i = 0; i < gradient.Size(); ++i) {
    Check(std::abs(gradient[i].x - expected_x) < mohidng::kDefaultTolerance, label + ": unexpected x-gradient.");
    Check(std::abs(gradient[i].y - expected_y) < mohidng::kDefaultTolerance, label + ": unexpected y-gradient.");
  }
}

void CheckWeighting(const mohidng::MeshView& mesh, mohidng::GradientWeightingPolicy weighting,
                    const std::string& label) {
  mohidng::GradientOptions options;
  options.weighting = std::move(weighting);

  const auto workspace = mohidng::BuildCellGradientWorkspace(mesh, options);
  Check(workspace.Size() == mesh.Cells().size(), label + ": unexpected workspace size.");
  for (const auto& stencil : workspace.Stencils()) {
    Check(stencil.well_conditioned, label + ": ill-conditioned stencil.");
    Check(stencil.coefficients.size() >= 2, label + ": insufficient cached coefficients.");
  }

  CheckGradient(mohidng::ComputeCellGradient(workspace, MakeConstantField(mesh)), 0.0, 0.0,
                label + " constant cached");
  CheckGradient(mohidng::ComputeCellGradient(workspace, MakeLinearField(mesh)), 1.0, 2.0,
                label + " linear cached");
  CheckGradient(mohidng::ComputeCellGradient(mesh, MakeLinearField(mesh), options), 1.0, 2.0,
                label + " linear one-shot");
}

}  // namespace

int main(int argc, char** argv) {
  Check(argc == 2, "Expected Voronoi mesh package path argument.");
  const auto mesh = mohidng::ReadVoronoiMeshPackage(std::filesystem::path(argv[1]));

  const auto methods = mohidng::RegisteredCellGradientMethods();
  Check(!methods.empty(), "No cell-gradient methods were registered.");

  CheckWeighting(mesh, mohidng::UniformWeightingPolicy(), "uniform WLS");
  CheckWeighting(mesh, mohidng::InverseDistanceWeightingPolicy(), "inverse-distance WLS");
  CheckWeighting(mesh, mohidng::InverseDistanceSquaredWeightingPolicy(),
                 "inverse-distance-squared WLS");

  return 0;
}
