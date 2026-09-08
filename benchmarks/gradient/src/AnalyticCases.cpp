// SPDX-License-Identifier: MPL-2.0

#include "GradientBenchmark.h"


namespace mohidng::benchmarks::gradient {

namespace {

// Evaluates the constant analytical scalar field:
//
//   phi(x,y) = 3
//
// The coordinates are unused because the field is spatially uniform.
[[nodiscard]] Real ConstantValue(
    Real,
    Real) {

  return
      static_cast<Real>(3.0);
}


// Evaluates the exact gradient of the constant analytical scalar field:
//
//   grad(phi) = (0,0)
[[nodiscard]] Vec2 ConstantGradient(
    Real,
    Real) {

  return Vec2{
      static_cast<Real>(0.0),
      static_cast<Real>(0.0)};
}


// Evaluates the linear analytical scalar field:
//
//   phi(x,y) = 2x - 3y + 1
[[nodiscard]] Real LinearValue(
    Real x,
    Real y) {

  constexpr Real coefficient_x =
      static_cast<Real>(2.0);

  constexpr Real coefficient_y =
      static_cast<Real>(-3.0);

  constexpr Real constant =
      static_cast<Real>(1.0);

  return
      coefficient_x * x +
      coefficient_y * y +
      constant;
}


// Evaluates the exact gradient of the linear analytical scalar field:
//
//   grad(phi) = (2,-3)
//
// The coordinates are unused because the gradient is spatially uniform.
[[nodiscard]] Vec2 LinearGradient(
    Real,
    Real) {

  return Vec2{
      static_cast<Real>(2.0),
      static_cast<Real>(-3.0)};
}

}  // namespace


// Returns the constant analytical verification case.
//
// The scalar field and its exact gradient are stored together so that both
// sampling and numerical-error evaluation use the same mathematical
// definition.
AnalyticalScalarCase MakeConstantCase() {

  AnalyticalScalarCase analytical_case;

  analytical_case.name =
      "constant";

  analytical_case.value =
      &ConstantValue;

  analytical_case.gradient =
      &ConstantGradient;

  return analytical_case;
}


// Returns the linear analytical verification case.
//
// The scalar field and its exact gradient are stored together so that both
// sampling and numerical-error evaluation use the same mathematical
// definition.
AnalyticalScalarCase MakeLinearCase() {

  AnalyticalScalarCase analytical_case;

  analytical_case.name =
      "linear";

  analytical_case.value =
      &LinearValue;

  analytical_case.gradient =
      &LinearGradient;

  return analytical_case;
}


// Samples an analytical scalar field at the centre of every finite-volume cell.
//
// CellIndex is currently used as the storage index of cell-centred fields. This
// preserves the indexing convention used by the MOHID-NG mesh and field
// infrastructure.
//
// The analytical case is expected to provide a valid scalar-value function.
ScalarField SampleScalarField(
    const MeshView& mesh,
    const AnalyticalScalarCase& analytical_case) {

  ScalarField field(
      analytical_case.name,
      CellFieldLocation(),
      static_cast<Size>(
          mesh.Cells().size()));

  for (const auto& cell :
       mesh.Cells()) {

    const Size index =
        static_cast<Size>(
            cell.id.value);

    field[index] =
        analytical_case.value(
            cell.centre.x,
            cell.centre.y);
  }

  return field;
}

}  // namespace mohidng::benchmarks::gradient