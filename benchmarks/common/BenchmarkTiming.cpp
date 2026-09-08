// SPDX-License-Identifier: MPL-2.0

#include "BenchmarkTiming.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <numeric>
#include <vector>

#include <boost/math/distributions/students_t.hpp>


namespace mohidng::benchmarks {

namespace {

// Returns a percentile from an ordered sample.
//
// Linear interpolation is used when the requested percentile lies between two
// measured values.
[[nodiscard]] Real Percentile(
    const std::vector<Real>& sorted,
    Real probability) {

  if (sorted.empty()) {
    return 0.0;
  }

  if (sorted.size() == 1) {
    return sorted.front();
  }

  const Real position =
      probability *
      static_cast<Real>(
          sorted.size() - 1);

  const Size lower =
      static_cast<Size>(
          std::floor(position));

  const Size upper =
      static_cast<Size>(
          std::ceil(position));

  const Real fraction =
      position -
      static_cast<Real>(
          lower);

  return
      sorted[lower] *
          (1.0 - fraction) +
      sorted[upper] *
          fraction;
}


// Returns the two-sided 95% Student-t critical value.
//
// For n measurements, the number of degrees of freedom is n - 1.
//
// Student's t distribution is used instead of the normal approximation because
// the variance of the measured execution time is estimated from the collected
// sample.
[[nodiscard]] Real StudentTCritical95(
    Size degrees_of_freedom) {

  if (degrees_of_freedom == 0) {
    return
        std::numeric_limits<Real>::infinity();
  }

  const boost::math::students_t_distribution<Real>
      distribution(
          static_cast<Real>(
              degrees_of_freedom));

  return
      boost::math::quantile(
          distribution,
          static_cast<Real>(0.975));
}

}  // namespace


// Updates all statistical quantities associated with the collected timing
// samples.
//
// Each entry in measurements represents the estimated execution time of one
// invocation of the measured operation, rather than the duration of the
// complete timing block.
void UpdateTimingStatistics(
    TimingResult& result,
    const std::vector<Real>& measurements) {

  result.n_measurements =
      static_cast<Size>(
          measurements.size());

  if (measurements.empty()) {
    return;
  }


  // ---------------------------------------------------------------------------
  // Arithmetic mean
  // ---------------------------------------------------------------------------

  const Real sum =
      std::accumulate(
          measurements.begin(),
          measurements.end(),
          Real{0.0});

  result.mean_time_s =
      sum /
      static_cast<Real>(
          measurements.size());


  // ---------------------------------------------------------------------------
  // Sample standard deviation and extrema
  // ---------------------------------------------------------------------------

  Real squared_difference_sum = 0.0;

  result.min_time_s =
      measurements.front();

  result.max_time_s =
      measurements.front();

  for (const Real value :
       measurements) {

    const Real difference =
        value -
        result.mean_time_s;

    squared_difference_sum +=
        difference *
        difference;

    result.min_time_s =
        std::min(
            result.min_time_s,
            value);

    result.max_time_s =
        std::max(
            result.max_time_s,
            value);
  }

  if (measurements.size() > 1) {

    result.std_time_s =
        std::sqrt(
            squared_difference_sum /
            static_cast<Real>(
                measurements.size() - 1));

  } else {

    result.std_time_s = 0.0;
  }


  // ---------------------------------------------------------------------------
  // Median and interquartile range
  // ---------------------------------------------------------------------------

  std::vector<Real> sorted =
      measurements;

  std::sort(
      sorted.begin(),
      sorted.end());

  result.median_time_s =
      Percentile(
          sorted,
          static_cast<Real>(0.5));

  result.iqr_time_s =
      Percentile(
          sorted,
          static_cast<Real>(0.75)) -
      Percentile(
          sorted,
          static_cast<Real>(0.25));


  // ---------------------------------------------------------------------------
  // Coefficient of variation
  // ---------------------------------------------------------------------------

  if (result.mean_time_s > 0.0) {

    result.coefficient_of_variation =
        result.std_time_s /
        result.mean_time_s;

  } else {

    result.coefficient_of_variation =
        0.0;
  }


  // ---------------------------------------------------------------------------
  // Student-t 95% confidence interval
  // ---------------------------------------------------------------------------

  if (measurements.size() > 1) {

    const Size degrees_of_freedom =
        static_cast<Size>(
            measurements.size() - 1);

    const Real critical_value =
        StudentTCritical95(
            degrees_of_freedom);

    const Real standard_error =
        result.std_time_s /
        std::sqrt(
            static_cast<Real>(
                measurements.size()));

    result.ci95_half_width_s =
        critical_value *
        standard_error;

  } else {

    result.ci95_half_width_s =
        std::numeric_limits<Real>::infinity();
  }


  // ---------------------------------------------------------------------------
  // Relative confidence interval
  // ---------------------------------------------------------------------------

  if (result.mean_time_s > 0.0) {

    result.ci95_relative =
        result.ci95_half_width_s /
        result.mean_time_s;

  } else {

    result.ci95_relative =
        std::numeric_limits<Real>::infinity();
  }
}

}  // namespace mohidng::benchmarks