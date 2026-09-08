// SPDX-License-Identifier: MPL-2.0

#include "GradientBenchmark.h"


namespace mohidng::benchmarks::gradient {

// Creates the constant scalar field used by the benchmark.
//
// The analytical field is:
//
//   phi(x,y) = 3
//
// Its exact gradient is:
//
//   grad(phi) = (0,0)
//
// This field provides a basic consistency check for the reconstruction
// operator. Any non-zero reconstructed gradient contributes directly to the
// reported numerical error.
ScalarField MakeConstantField(
    const MeshView& mesh) {

  ScalarField field(
      "constant",
      CellFieldLocation(),
      static_cast<Size>(
          mesh.Cells().size()));

  for (Size index = 0;
       index < field.Size();
       ++index) {

    field[index] = static_cast<Real>(3.0);
  }

  return field;
}


// Creates the linear scalar field used by the benchmark.
//
// The analytical field is:
//
//   phi(x,y) = 2x - 3y + 1
//
// Its exact gradient is uniform:
//
//   grad(phi) = (2,-3)
//
// The field is evaluated at the geometric centre of each finite volume.
//
// CellIndex is currently used as the storage index of cell-centred fields.
// The benchmark therefore preserves the indexing convention already used by
// the MOHID-NG mesh and field infrastructure.
ScalarField MakeLinearField(
    const MeshView& mesh) {

  constexpr Real coefficient_x =
      static_cast<Real>(2.0);

  constexpr Real coefficient_y =
      static_cast<Real>(-3.0);

  constexpr Real constant =
      static_cast<Real>(1.0);

  ScalarField field(
      "linear",
      CellFieldLocation(),
      static_cast<Size>(
          mesh.Cells().size()));

  for (const auto& cell :
       mesh.Cells()) {

    const Size index =
        static_cast<Size>(
            cell.id.value);

    field[index] =
        coefficient_x * cell.centre.x +
        coefficient_y * cell.centre.y +
        constant;
  }

  return field;
}

}  // namespace mohidng::benchmarks::gradient