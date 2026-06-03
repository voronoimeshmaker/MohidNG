#include "MohidNG/Numerics/Gradient.h"

#include <cmath>

#include "MohidNG/Core/Error.h"

namespace mohidng {
namespace {

struct NormalEquation2D {
  double a00 = 0.0;
  double a01 = 0.0;
  double a11 = 0.0;
  double b0 = 0.0;
  double b1 = 0.0;
};

Vec2 Solve2x2(const NormalEquation2D& equation, double regularisation) {
  const double a00 = equation.a00 + regularisation;
  const double a11 = equation.a11 + regularisation;
  const double determinant = a00 * a11 - equation.a01 * equation.a01;
  Require(std::abs(determinant) > 1.0e-24, "Singular local gradient reconstruction system.");
  return Vec2{(equation.b0 * a11 - equation.b1 * equation.a01) / determinant,
              (a00 * equation.b1 - equation.a01 * equation.b0) / determinant};
}

}  // namespace

Vector2Field ComputeCellGradient(const MeshView& mesh, const ScalarField& scalar,
                                 const GradientOptions& options) {
  Require(options.method == GradientMethod::kWeightedLeastSquaresId2,
          "Only weighted least-squares ID2 is implemented in the bootstrap core.");
  Require(scalar.Location() == FieldLocation::kCell, "Gradient input must be a cell field.");
  Require(scalar.Size() == mesh.Cells().size(), "Scalar field size does not match mesh cell count.");

  Vector2Field gradients("grad_" + scalar.Name(), FieldLocation::kCell, scalar.Size());

  for (const auto& cell : mesh.Cells()) {
    NormalEquation2D normal;
    const auto neighbours = mesh.CellNeighbours(cell.id);
    Require(neighbours.size() >= 2, "At least two neighbours are required for a 2-D gradient.");

    for (const auto neighbour_id : neighbours) {
      const auto& neighbour = mesh.Cell(neighbour_id);
      const double dx = neighbour.centre.x - cell.centre.x;
      const double dy = neighbour.centre.y - cell.centre.y;
      const double distance_squared = dx * dx + dy * dy;
      Require(distance_squared > 0.0, "Degenerate cell-centre distance in gradient reconstruction.");
      const double weight = 1.0 / distance_squared;
      const double delta = scalar[static_cast<std::size_t>(neighbour.id.value)] -
                           scalar[static_cast<std::size_t>(cell.id.value)];

      normal.a00 += weight * dx * dx;
      normal.a01 += weight * dx * dy;
      normal.a11 += weight * dy * dy;
      normal.b0 += weight * dx * delta;
      normal.b1 += weight * dy * delta;
    }

    gradients[static_cast<std::size_t>(cell.id.value)] = Solve2x2(normal, options.regularisation);
  }

  return gradients;
}

}  // namespace mohidng
