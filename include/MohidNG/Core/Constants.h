#pragma once

#include <numbers>

#include <MohidNG/Core/Types.h>


namespace mohidng {

inline constexpr Real kPi = std::numbers::pi_v<Real>;   // Mathematical pi for geometric and trigonometric calculations.
inline constexpr Real kEpsilon = 1.0e-30;               // Practical zero used to avoid exact comparisons with null values.
inline constexpr Real kDefaultTolerance = 1.0e-12;      // Default numerical tolerance for validation and regression checks.
inline constexpr Real kSingularTolerance = 1.0e-14;     // Threshold used to detect nearly singular numerical systems.

}  // namespace mohidng
