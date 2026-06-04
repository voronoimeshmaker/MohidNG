#include <cmath>
#include <string_view>
#include <utility>
#include <vector>

#include <MohidNG/Core/Error.h>
#include <MohidNG/Core/ID.h>
#include <MohidNG/Mesh/MeshView.h>
#include <MohidNG/Numerics/Gradient/Gradient.h>


namespace mohidng {
namespace {

struct NormalEquation2D {
  Real a00 = 0.0;
  Real a01 = 0.0;
  Real a11 = 0.0;
};

struct NeighbourGeometry {
  CellIndex neighbour{};
  Real dx = 0.0;
  Real dy = 0.0;
  Real weight = 1.0;
};

class WeightedLeastSquaresNormalEquationsGradientMethod {
 public:
  DefineIdentity("MohidNG.Numerics.Gradient.WeightedLeastSquaresNormalEquationsGradientMethod")

  [[nodiscard]] static constexpr std::string_view Name() {
    return "weighted_least_squares_normal_equations";
  }

  [[nodiscard]] static CellGradientWorkspace BuildWorkspace(const MeshView& mesh,
                                                            const GradientOptions& options) {
    std::vector<CellGradientStencil> stencils;
    stencils.reserve(mesh.Cells().size());
    for (const auto& cell : mesh.Cells()) {
      stencils.push_back(BuildStencilForCell(mesh, cell, options));
    }
    return CellGradientWorkspace(std::move(stencils), std::string(Name()), options.weighting.Name());
  }

 private:
  [[nodiscard]] static CellGradientStencil BuildStencilForCell(const MeshView& mesh,
                                                               const Cell2D& cell,
                                                               const GradientOptions& options) {
    CellGradientStencil stencil;
    stencil.cell = cell.id;

    NormalEquation2D normal;
    std::vector<NeighbourGeometry> neighbours;
    const auto neighbour_ids = mesh.CellNeighbours(cell.id);
    neighbours.reserve(neighbour_ids.size());

    for (const auto neighbour_id : neighbour_ids) {
      const auto& neighbour = mesh.Cell(neighbour_id);
      const Real dx = neighbour.centre.x - cell.centre.x;
      const Real dy = neighbour.centre.y - cell.centre.y;
      const Real distance_squared = dx * dx + dy * dy;
      RequireClass(WeightedLeastSquaresNormalEquationsGradientMethod, distance_squared > 0.0,
                   "numerics.degenerate_geometry",
                   "Degenerate cell-centre distance in gradient reconstruction.");

      const GradientNeighbourGeometry geometry{cell.id, neighbour_id, Vec2{dx, dy},
                                               distance_squared};
      const Real weight = options.weighting.Weight(geometry);
      normal.a00 += weight * dx * dx;
      normal.a01 += weight * dx * dy;
      normal.a11 += weight * dy * dy;
      neighbours.push_back(NeighbourGeometry{neighbour_id, dx, dy, weight});
    }

    stencil.determinant = normal.a00 * normal.a11 - normal.a01 * normal.a01;
    if (neighbours.size() < 2 || std::abs(stencil.determinant) <= options.singular_tolerance) {
      return stencil;
    }

    const Real inv00 = normal.a11 / stencil.determinant;
    const Real inv01 = -normal.a01 / stencil.determinant;
    const Real inv11 = normal.a00 / stencil.determinant;

    stencil.coefficients.reserve(neighbours.size());
    for (const auto& neighbour : neighbours) {
      const Real weighted_dx = neighbour.weight * neighbour.dx;
      const Real weighted_dy = neighbour.weight * neighbour.dy;
      stencil.coefficients.push_back(CellGradientCoefficient{
          neighbour.neighbour,
          Vec2{inv00 * weighted_dx + inv01 * weighted_dy,
               inv01 * weighted_dx + inv11 * weighted_dy}});
    }
    stencil.well_conditioned = true;
    return stencil;
  }
};

}  // namespace

void RegisterWeightedLeastSquaresNormalEquationsGradientMethod() {
  RegisterCellGradientMethod<WeightedLeastSquaresNormalEquationsGradientMethod>();
}

}  // namespace mohidng

