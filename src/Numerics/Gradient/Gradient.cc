#include <algorithm>
#include <cmath>
#include <string>
#include <unordered_map>
#include <utility>

#include <MohidNG/Core/Constants.h>
#include <MohidNG/Core/Error.h>
#include <MohidNG/Core/Logger.h>
#include <MohidNG/Numerics/Gradient/BuiltinMethods.h>
#include <MohidNG/Numerics/Gradient/Gradient.h>


namespace mohidng {
namespace {

[[nodiscard]] std::unordered_map<std::string, CellGradientWorkspaceBuilder>& MethodRegistry() {
  static std::unordered_map<std::string, CellGradientWorkspaceBuilder> registry;
  return registry;
}

void EnsureDefaultMethodsRegistered() {
  RegisterWeightedLeastSquaresNormalEquationsGradientMethod();
  RegisterClassicalGreenGaussGradientMethod();
}

}  // namespace

GradientWeightingPolicy::GradientWeightingPolicy(std::string name, Function function)
    : name_(std::move(name)), function_(std::move(function)) {
  RequireClass(GradientWeightingPolicy, static_cast<bool>(function_), "core.requirement_failed",
               "Gradient weighting policy requires a callable function.");
}

const std::string& GradientWeightingPolicy::Name() const { return name_; }

Real GradientWeightingPolicy::Weight(const GradientNeighbourGeometry& geometry) const {
  return function_(geometry);
}

Real UniformGradientWeighting::operator()(const GradientNeighbourGeometry&) const { return 1.0; }

Real InverseDistanceGradientWeighting::operator()(const GradientNeighbourGeometry& geometry) const {
  return 1.0 / std::sqrt(geometry.distance_squared + kEpsilon);
}

Real InverseDistanceSquaredGradientWeighting::operator()(
    const GradientNeighbourGeometry& geometry) const {
  return 1.0 / (geometry.distance_squared + kEpsilon);
}

GradientWeightingPolicy UniformWeightingPolicy() {
  return MakeGradientWeightingPolicy(UniformGradientWeighting{});
}

GradientWeightingPolicy InverseDistanceWeightingPolicy() {
  return MakeGradientWeightingPolicy(InverseDistanceGradientWeighting{});
}

GradientWeightingPolicy InverseDistanceSquaredWeightingPolicy() {
  return MakeGradientWeightingPolicy(InverseDistanceSquaredGradientWeighting{});
}

CellGradientWorkspace::CellGradientWorkspace(std::vector<CellGradientStencil> stencils,
                                             std::string method, std::string weighting)
    : stencils_(std::move(stencils)), method_(std::move(method)), weighting_(std::move(weighting)) {}

std::size_t CellGradientWorkspace::Size() const { return stencils_.size(); }

const std::string& CellGradientWorkspace::MethodName() const { return method_; }

const std::string& CellGradientWorkspace::WeightingName() const { return weighting_; }

const CellGradientStencil& CellGradientWorkspace::Stencil(CellIndex cell) const {
  MOHIDNG_TRACE_CLASS(CellGradientWorkspace::ClassId(), "Stencil");
  RequireClass(CellGradientWorkspace, mohidng::IsValid(cell), "mesh.invalid_connectivity",
               "Invalid cell index.");
  RequireClass(CellGradientWorkspace, static_cast<std::size_t>(cell.value) < stencils_.size(),
               "mesh.invalid_connectivity", "Cell gradient stencil index out of range.");
  return stencils_.at(static_cast<std::size_t>(cell.value));
}

const std::vector<CellGradientStencil>& CellGradientWorkspace::Stencils() const { return stencils_; }

CellGradientDiagnostics CellGradientWorkspace::Diagnostics() const {
  CellGradientDiagnostics diagnostics;
  diagnostics.method = method_;
  diagnostics.weighting = weighting_;
  diagnostics.cell_count = stencils_.size();
  for (const auto& stencil : stencils_) {
    diagnostics.coefficient_count += stencil.coefficients.size();
    if (stencil.well_conditioned) {
      ++diagnostics.well_conditioned_count;
    } else {
      ++diagnostics.ill_conditioned_count;
    }
  }
  return diagnostics;
}

void RegisterCellGradientMethod(std::string name, CellGradientWorkspaceBuilder builder) {
  MOHIDNG_TRACE_SCOPE("RegisterCellGradientMethod");
  Require(!name.empty(), "core.requirement_failed", "Gradient method name cannot be empty.");
  Require(static_cast<bool>(builder), "core.requirement_failed",
          "Gradient method builder cannot be empty.");
  auto& registry = MethodRegistry();
  registry[std::move(name)] = std::move(builder);
}

std::vector<std::string> RegisteredCellGradientMethods() {
  EnsureDefaultMethodsRegistered();
  std::vector<std::string> names;
  names.reserve(MethodRegistry().size());
  for (const auto& [name, builder] : MethodRegistry()) {
    (void)builder;
    names.push_back(name);
  }
  std::ranges::sort(names);
  return names;
}

CellGradientWorkspace BuildCellGradientWorkspace(const MeshView& mesh, const GradientOptions& options) {
  MOHIDNG_TRACE_SCOPE("BuildCellGradientWorkspace");
  EnsureDefaultMethodsRegistered();
  const auto& registry = MethodRegistry();
  const auto it = registry.find(options.method);
  Require(it != registry.end(), "numerics.unsupported_gradient_method", options.method);
  return it->second(mesh, options);
}

Vector2Field ComputeCellGradient(const CellGradientWorkspace& workspace, const ScalarField& scalar) {
  MOHIDNG_TRACE_SCOPE("ComputeCellGradient cached");
  Require(scalar.Location() == CellFieldLocation(), "numerics.gradient_requires_cell_field",
          scalar.Name());
  Require(scalar.Size() == workspace.Size(), "field.invalid_size",
          "Scalar field size does not match cell gradient workspace size.");

  Vector2Field gradients("grad_" + scalar.Name(), CellFieldLocation(), scalar.Size());

  for (const auto& stencil : workspace.Stencils()) {
    Require(stencil.well_conditioned, "numerics.singular_least_squares",
            "cell=" + std::to_string(stencil.cell.value));
    const auto cell_index = static_cast<std::size_t>(stencil.cell.value);
    const Real centre_value = scalar[cell_index];
    Vec2 gradient{};
    for (const auto& coefficient : stencil.coefficients) {
      const auto neighbour_index = static_cast<std::size_t>(coefficient.neighbour.value);
      const Real delta = scalar[neighbour_index] - centre_value;
      gradient.x += coefficient.coefficient.x * delta;
      gradient.y += coefficient.coefficient.y * delta;
    }
    gradients[cell_index] = gradient;
  }

  return gradients;
}

Vector2Field ComputeCellGradient(const MeshView& mesh, const ScalarField& scalar,
                                 const GradientOptions& options) {
  MOHIDNG_TRACE_SCOPE("ComputeCellGradient");
  Require(scalar.Location() == CellFieldLocation(), "numerics.gradient_requires_cell_field",
          scalar.Name());
  Require(scalar.Size() == mesh.Cells().size(), "field.invalid_size",
          "Scalar field size does not match mesh cell count.");
  const auto workspace = BuildCellGradientWorkspace(mesh, options);
  return ComputeCellGradient(workspace, scalar);
}

}  // namespace mohidng
