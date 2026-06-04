#include <algorithm>
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
    linear[static_cast<std::size_t>(cell.id.value)] = 2.0 * cell.centre.x - 3.0 * cell.centre.y + 5.0;
  }
  return linear;
}

mohidng::ScalarField MakeConstantField(const mohidng::MeshView& mesh) {
  mohidng::ScalarField constant("constant", mohidng::CellFieldLocation(), mesh.Cells().size());
  for (std::size_t i = 0; i < constant.Size(); ++i) {
    constant[i] = 7.0;
  }
  return constant;
}

void CheckLinearGradient(const mohidng::Vector2Field& gradient, const std::string& label) {
  for (std::size_t i = 0; i < gradient.Size(); ++i) {
    Check(std::abs(gradient[i].x - 2.0) < mohidng::kDefaultTolerance, label + ": unexpected x-gradient.");
    Check(std::abs(gradient[i].y + 3.0) < mohidng::kDefaultTolerance, label + ": unexpected y-gradient.");
  }
}

void CheckZeroGradient(const mohidng::Vector2Field& gradient, const std::string& label) {
  for (std::size_t i = 0; i < gradient.Size(); ++i) {
    Check(std::abs(gradient[i].x) < mohidng::kDefaultTolerance, label + ": non-zero x-gradient.");
    Check(std::abs(gradient[i].y) < mohidng::kDefaultTolerance, label + ": non-zero y-gradient.");
  }
}

mohidng::CellGradientWorkspace CopyDefaultWorkspaceBuilder(
    const mohidng::MeshView& mesh, const mohidng::GradientOptions& options) {
  mohidng::GradientOptions forwarded = options;
  forwarded.method = "weighted_least_squares_normal_equations";
  return mohidng::BuildCellGradientWorkspace(mesh, forwarded);
}

}  // namespace

int main(int argc, char** argv) {
  Check(argc == 2, "Expected Voronoi mesh package path argument.");
  const auto mesh = mohidng::ReadVoronoiMeshPackage(std::filesystem::path(argv[1]));
  const auto linear = MakeLinearField(mesh);

  mohidng::GradientOptions custom_weight;
  custom_weight.weighting = mohidng::GradientWeightingPolicy(
      "test_uniform_lambda", [](const mohidng::GradientNeighbourGeometry&) { return 1.0; });
  CheckLinearGradient(mohidng::ComputeCellGradient(mesh, linear, custom_weight),
                      "custom lambda weighting");

  mohidng::RegisterCellGradientMethod("test_forwarded_wls", CopyDefaultWorkspaceBuilder);
  const auto methods = mohidng::RegisteredCellGradientMethods();
  Check(std::ranges::find(methods, "weighted_least_squares_normal_equations") != methods.end(),
        "Default WLS method was not listed.");
  Check(std::ranges::find(methods, "classical_green_gauss") != methods.end(),
        "Classical Green-Gauss method was not listed.");
  Check(std::ranges::find(methods, "test_forwarded_wls") != methods.end(),
        "Registered gradient method was not listed.");

  mohidng::GradientOptions custom_method;
  custom_method.method = "test_forwarded_wls";
  custom_method.weighting = mohidng::InverseDistanceSquaredWeightingPolicy();
  const auto workspace = mohidng::BuildCellGradientWorkspace(mesh, custom_method);
  Check(workspace.Size() == mesh.Cells().size(), "Custom method returned wrong workspace size.");
  CheckLinearGradient(mohidng::ComputeCellGradient(workspace, linear), "registered method");

  mohidng::GradientOptions green_gauss;
  green_gauss.method = "classical_green_gauss";
  const auto green_gauss_workspace = mohidng::BuildCellGradientWorkspace(mesh, green_gauss);
  Check(green_gauss_workspace.Size() == mesh.Cells().size(),
        "Classical Green-Gauss returned wrong workspace size.");
  CheckZeroGradient(mohidng::ComputeCellGradient(green_gauss_workspace, MakeConstantField(mesh)),
                    "classical Green-Gauss constant field");

  return 0;
}
