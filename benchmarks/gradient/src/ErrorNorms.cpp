// SPDX-License-Identifier: MPL-2.0

#include "GradientBenchmark.h"

#include <algorithm>
#include <cmath>


namespace mohidng::benchmarks::gradient {

namespace {

// Adds one local Euclidean gradient error to the accumulated norms.
//
// During accumulation:
//
//   L1   stores the sum of local errors;
//   L2   stores the sum of squared local errors;
//   Linf stores the maximum local error.
//
// L1 and L2 are normalised after all cells have been processed.
void AccumulateError(
    ErrorNorms& norms,
    Real error) {

  norms.l1 +=
      error;

  norms.l2 +=
      error *
      error;

  norms.linf =
      std::max(
          norms.linf,
          error);

  ++norms.count;
}


// Converts the accumulated sums into the reported discrete norms.
//
// The benchmark defines:
//
//   L1   = arithmetic mean of the local Euclidean errors
//
//   L2   = root-mean-square of the local Euclidean errors
//
//   Linf = maximum local Euclidean error
void FinaliseError(
    ErrorNorms& norms) {

  if (norms.count == 0) {
    return;
  }

  const Real count =
      static_cast<Real>(
          norms.count);

  norms.l1 /=
      count;

  norms.l2 =
      std::sqrt(
          norms.l2 /
          count);
}


// Returns the Euclidean magnitude of a two-dimensional vector.
[[nodiscard]] Real VectorNorm(
    Real x,
    Real y) {

  return
      std::sqrt(
          x * x +
          y * y);
}

}  // namespace


// Computes gradient reconstruction errors against an analytical scalar case.
//
// For every finite-volume cell:
//
//   1. the exact analytical gradient is evaluated at the cell centre;
//   2. the numerical and analytical gradients are subtracted;
//   3. the Euclidean magnitude of that difference is calculated;
//   4. the local error is accumulated into L1, L2 and Linf.
//
// The same implementation is therefore used for constant, linear and future
// manufactured analytical scalar fields.
//
// The numerical gradient field is assumed to follow the same cell-index storage
// convention as the MOHID-NG mesh and scalar-field infrastructure.
ErrorNorms ComputeGradientError(
    const MeshView& mesh,
    const Vector2Field& numerical_gradient,
    const AnalyticalScalarCase& analytical_case) {

  ErrorNorms norms;

  for (const auto& cell :
       mesh.Cells()) {

    const Size index =
        static_cast<Size>(
            cell.id.value);

    const Vec2 exact_gradient =
        analytical_case.gradient(
            cell.centre.x,
            cell.centre.y);

    const Real error_x =
        numerical_gradient[index].x -
        exact_gradient.x;

    const Real error_y =
        numerical_gradient[index].y -
        exact_gradient.y;

    const Real error =
        VectorNorm(
            error_x,
            error_y);

    AccumulateError(
        norms,
        error);
  }

  FinaliseError(
      norms);

  return norms;
}

}  // namespace mohidng::benchmarks::gradient