#pragma once

#include <vector>

#include "MohidNG/Fields/Field.h"
#include "MohidNG/Mesh/MeshView.h"

namespace mohidng {

enum class GradientMethod { kWeightedLeastSquaresId2 };

struct GradientOptions {
  GradientMethod method = GradientMethod::kWeightedLeastSquaresId2;
  double regularisation = 1.0e-30;
};

[[nodiscard]] Vector2Field ComputeCellGradient(const MeshView& mesh, const ScalarField& scalar,
                                               const GradientOptions& options = {});

}  // namespace mohidng
