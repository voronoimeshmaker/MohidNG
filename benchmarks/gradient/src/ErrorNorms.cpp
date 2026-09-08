// SPDX-License-Identifier: MPL-2.0

#include <algorithm>
#include <cmath>

#include "GradientBenchmark.h"


namespace mohidng::benchmarks::gradient {

namespace {

// Adds one local Euclidean gradient error to the accumulated norms.
//
// L1 stores the sum of local errors until finalisation.
// L2 stores the sum of squared local errors until finalisation.
// Linf is updated directly as the maximum local error.
void AccumulateError(
    ErrorNorms& norms,
    Real error) {

  norms.l1 += error;
  norms.l2 += error * error;
  norms.linf = std::max(norms.linf, error);

  ++norms.count;
}


// Converts accumulated sums into the reported discrete norms.
//
// The benchmark defines:
//
//   L1   = arithmetic mean of the local Euclidean errors
//
//   L2   = root-mean-square of the local Euclidean errors
//
//   Linf = maximum local Euclidean error
//
// Linf is already complete during accumulation and therefore requires no
// additional processing here.
void FinaliseError(
    ErrorNorms& norms) {

  if (norms.count == 0) {
    return;
  }

  const Real count =
      static_cast<Real>(norms.count);

  norms.l1 /= count;

  norms.l2 =
      std::sqrt(
          norms.l2 / count);
}


// Returns the Euclidean magnitude of a two-dimensional vector.
[[nodiscard]] Real VectorNorm(
    Real x,
    Real y) {

  return std::sqrt(
      x * x +
      y * y);
}

}  // namespace


// Computes the reconstruction error for the constant scalar field.
//
// The analytical field is:
//
//   phi(x,y) = 3
//
// and its exact gradient is:
//
//   grad(phi) = (0,0)
//
// Therefore, the local error is simply the magnitude of the reconstructed
// gradient at each cell.
ErrorNorms ConstantGradientError(
    const Vector2Field& gradient) {

  ErrorNorms norms;

  for (Size i = 0;
       i < gradient.Size();
       ++i) {

    const Real error =
        VectorNorm(
            gradient[i].x,
            gradient[i].y);

    AccumulateError(
        norms,
        error);
  }

  FinaliseError(norms);

  return norms;
}


// Computes the reconstruction error for the linear scalar field.
//
// The analytical field is:
//
//   phi(x,y) = 2x - 3y + 1
//
// and its exact gradient is:
//
//   grad(phi) = (2,-3)
//
// The local error is the Euclidean norm of the difference between the
// reconstructed and exact gradients.
ErrorNorms LinearGradientError(
    const Vector2Field& gradient) {

  constexpr Real exact_x = 2.0;
  constexpr Real exact_y = -3.0;

  ErrorNorms norms;

  for (Size i = 0;
       i < gradient.Size();
       ++i) {

    const Real error_x =
        gradient[i].x - exact_x;

    const Real error_y =
        gradient[i].y - exact_y;

    const Real error =
        VectorNorm(
            error_x,
            error_y);

    AccumulateError(
        norms,
        error);
  }

  FinaliseError(norms);

  return norms;
}

}  // namespace mohidng::benchmarks::gradient