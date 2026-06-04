#pragma once

#include <functional>
#include <string>
#include <string_view>
#include <vector>

#include <MohidNG/Core/Constants.h>
#include <MohidNG/Core/ID.h>
#include <MohidNG/Fields/Field.h>
#include <MohidNG/Mesh/MeshView.h>


namespace mohidng {

struct GradientNeighbourGeometry {
  CellIndex cell{};
  CellIndex neighbour{};
  Vec2 delta{};
  Real distance_squared = 0.0;
};

class GradientWeightingPolicy {
 public:
  DefineIdentity("MohidNG.Numerics.Gradient.GradientWeightingPolicy")

  using Function = std::function<Real(const GradientNeighbourGeometry&)>;

  GradientWeightingPolicy(std::string name, Function function);

  [[nodiscard]] const std::string& Name() const;
  [[nodiscard]] Real Weight(const GradientNeighbourGeometry& geometry) const;

 private:
  std::string name_;
  Function function_;
};

struct UniformGradientWeighting {
  [[nodiscard]] static constexpr std::string_view Name() { return "uniform"; }
  [[nodiscard]] Real operator()(const GradientNeighbourGeometry& geometry) const;
};

struct InverseDistanceGradientWeighting {
  [[nodiscard]] static constexpr std::string_view Name() { return "inverse_distance"; }
  [[nodiscard]] Real operator()(const GradientNeighbourGeometry& geometry) const;
};

struct InverseDistanceSquaredGradientWeighting {
  [[nodiscard]] static constexpr std::string_view Name() { return "inverse_distance_squared"; }
  [[nodiscard]] Real operator()(const GradientNeighbourGeometry& geometry) const;
};

template <typename Weighting>
[[nodiscard]] GradientWeightingPolicy MakeGradientWeightingPolicy(Weighting weighting = {}) {
  return GradientWeightingPolicy(std::string(Weighting::Name()),
                                 [weighting](const GradientNeighbourGeometry& geometry) {
                                   return weighting(geometry);
                                 });
}

[[nodiscard]] GradientWeightingPolicy UniformWeightingPolicy();
[[nodiscard]] GradientWeightingPolicy InverseDistanceWeightingPolicy();
[[nodiscard]] GradientWeightingPolicy InverseDistanceSquaredWeightingPolicy();

struct GradientOptions {
  std::string method = "weighted_least_squares_normal_equations";
  GradientWeightingPolicy weighting = InverseDistanceSquaredWeightingPolicy();
  Real singular_tolerance = kSingularTolerance;
};

struct CellGradientCoefficient {
  CellIndex neighbour{};
  Vec2 coefficient{};
};

struct CellGradientStencil {
  CellIndex cell{};
  Real determinant = 0.0;
  bool well_conditioned = false;
  std::vector<CellGradientCoefficient> coefficients;
};

struct CellGradientDiagnostics {
  std::string method;
  std::string weighting;
  std::size_t cell_count = 0;
  std::size_t well_conditioned_count = 0;
  std::size_t ill_conditioned_count = 0;
  std::size_t coefficient_count = 0;
};

class CellGradientWorkspace {
 public:
  DefineIdentity("MohidNG.Numerics.Gradient.CellGradientWorkspace")

  CellGradientWorkspace() = default;
  CellGradientWorkspace(std::vector<CellGradientStencil> stencils, std::string method,
                        std::string weighting);

  [[nodiscard]] std::size_t Size() const;
  [[nodiscard]] const std::string& MethodName() const;
  [[nodiscard]] const std::string& WeightingName() const;
  [[nodiscard]] const CellGradientStencil& Stencil(CellIndex cell) const;
  [[nodiscard]] const std::vector<CellGradientStencil>& Stencils() const;
  [[nodiscard]] CellGradientDiagnostics Diagnostics() const;

 private:
  std::vector<CellGradientStencil> stencils_;
  std::string method_;
  std::string weighting_;
};

using CellGradientWorkspaceBuilder =
    std::function<CellGradientWorkspace(const MeshView&, const GradientOptions&)>;

void RegisterCellGradientMethod(std::string name, CellGradientWorkspaceBuilder builder);

template <typename Method>
void RegisterCellGradientMethod() {
  RegisterCellGradientMethod(std::string(Method::Name()), Method::BuildWorkspace);
}
[[nodiscard]] std::vector<std::string> RegisteredCellGradientMethods();

[[nodiscard]] CellGradientWorkspace BuildCellGradientWorkspace(
    const MeshView& mesh, const GradientOptions& options = {});

[[nodiscard]] Vector2Field ComputeCellGradient(const CellGradientWorkspace& workspace,
                                               const ScalarField& scalar);

[[nodiscard]] Vector2Field ComputeCellGradient(const MeshView& mesh, const ScalarField& scalar,
                                               const GradientOptions& options = {});

}  // namespace mohidng

