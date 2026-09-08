// SPDX-License-Identifier: MPL-2.0
#pragma once

#include <algorithm>
#include <chrono>
#include <cmath>
#include <limits>
#include <string>
#include <vector>

#include <MohidNG/Core/Types.h>


namespace mohidng::benchmarks {

// =============================================================================
// Benchmark timing configuration
// =============================================================================

// Controls the statistical measurement procedure shared by MOHID-NG
// preparatory benchmarks.
//
// Very short operations are grouped into timing blocks so that timer
// resolution and timing-call overhead do not dominate the measurements.
//
// Before collecting samples, warm-up executions are performed.
//
// Sampling terminates when either:
//
//   - the relative 95% confidence interval reaches the requested target; or
//   - max_measurements is reached.
//
// The confidence interval is calculated using Student's t distribution.
struct TimingSettings {
  Size warmup = 5;
  Size min_measurements = 30;
  Size max_measurements = 500;
  Size max_block_size = 10000;

  Real target_block_seconds = 0.1;
  Real target_ci95_relative = 0.02;
};


// Stores the statistical result of one measured benchmark operation.
//
// Every reported time corresponds to one invocation of the measured operation,
// even when several invocations are grouped into a timing block.
struct TimingResult {
  Size block_size = 1;
  Size n_measurements = 0;

  Real mean_time_s = 0.0;
  Real std_time_s = 0.0;
  Real median_time_s = 0.0;
  Real iqr_time_s = 0.0;
  Real min_time_s = 0.0;
  Real max_time_s = 0.0;

  Real coefficient_of_variation = 0.0;
  Real ci95_half_width_s = 0.0;
  Real ci95_relative = 0.0;

  Real checksum = 0.0;

  std::string status = "not_run";
};


// =============================================================================
// Statistical support
// =============================================================================

// Updates the statistical quantities stored in a timing result.
//
// This function is implemented in BenchmarkTiming.cpp because it does not
// depend on the measured kernel.
//
// The 95% confidence interval uses Student's t distribution.
void UpdateTimingStatistics(
    TimingResult& result,
    const std::vector<Real>& measurements);


// =============================================================================
// Generic benchmark execution
// =============================================================================

namespace detail {

using BenchmarkClock =
    std::chrono::steady_clock;


// Executes one timing block.
//
// The kernel must return a value convertible to Real. The returned values are
// accumulated into a checksum so that the measured computation remains
// observable.
template <class Kernel>
Real RunTimingBlock(
    Kernel&& kernel,
    Size block_size) {

  Real accumulated = 0.0;

  for (Size i = 0;
       i < block_size;
       ++i) {

    accumulated +=
        static_cast<Real>(
            kernel());
  }

  return accumulated;
}


// Selects a timing block size.
//
// The block starts with one invocation and grows by one order of magnitude
// until either:
//
//   - target_block_seconds is reached; or
//   - max_block_size is reached.
//
// Calibration executions contribute to the checksum but not to the statistical
// samples.
template <class Kernel>
Size ChooseTimingBlockSize(
    Kernel&& kernel,
    const TimingSettings& settings,
    Real& checksum) {

  Size block_size = 1;

  while (true) {

    const auto start =
        BenchmarkClock::now();

    checksum +=
        RunTimingBlock(
            kernel,
            block_size);

    const Real elapsed_seconds =
        static_cast<Real>(
            std::chrono::duration<double>(
                BenchmarkClock::now() - start)
                .count());

    if (elapsed_seconds >=
            settings.target_block_seconds ||
        block_size >=
            settings.max_block_size) {

      return block_size;
    }

    const Size next_block_size =
        block_size * 10;

    block_size =
        std::min(
            next_block_size,
            settings.max_block_size);
  }
}

}  // namespace detail


// Measures an arbitrary benchmark kernel.
//
// Procedure:
//
//   1. determine a suitable timing block size;
//   2. execute warm-up blocks;
//   3. collect repeated timing samples;
//   4. calculate statistics;
//   5. stop when the requested CI95 precision is reached or the configured
//      measurement limit is reached.
//
// The kernel must return a value convertible to Real. This value is used only
// to construct the benchmark checksum.
template <class Kernel>
[[nodiscard]] TimingResult MeasureBenchmark(
    Kernel&& kernel,
    const TimingSettings& settings = {}) {

  TimingResult result;

  Real checksum = 0.0;

  result.block_size =
      detail::ChooseTimingBlockSize(
          kernel,
          settings,
          checksum);


  // ---------------------------------------------------------------------------
  // Warm-up
  // ---------------------------------------------------------------------------

  for (Size i = 0;
       i < settings.warmup;
       ++i) {

    checksum +=
        detail::RunTimingBlock(
            kernel,
            result.block_size);
  }


  // ---------------------------------------------------------------------------
  // Statistical measurements
  // ---------------------------------------------------------------------------

  std::vector<Real> measurements;

  measurements.reserve(
      static_cast<std::size_t>(
          settings.max_measurements));

  while (measurements.size() <
         static_cast<std::size_t>(
             settings.max_measurements)) {

    const auto start =
        detail::BenchmarkClock::now();

    checksum +=
        detail::RunTimingBlock(
            kernel,
            result.block_size);

    const Real elapsed_seconds =
        static_cast<Real>(
            std::chrono::duration<double>(
                detail::BenchmarkClock::now() - start)
                .count());

    const Real time_per_operation =
        elapsed_seconds /
        static_cast<Real>(
            result.block_size);

    measurements.push_back(
        time_per_operation);

    if (measurements.size() >=
        static_cast<std::size_t>(
            settings.min_measurements)) {

      UpdateTimingStatistics(
          result,
          measurements);

      if (result.ci95_relative <=
          settings.target_ci95_relative) {

        result.status =
            "converged";

        break;
      }
    }
  }


  // ---------------------------------------------------------------------------
  // Non-converged termination
  // ---------------------------------------------------------------------------

  if (result.status !=
      "converged") {

    UpdateTimingStatistics(
        result,
        measurements);

    result.status =
        "max_measurements";
  }

  result.checksum =
      checksum;

  return result;
}

}  // namespace mohidng::benchmarks