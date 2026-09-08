// SPDX-License-Identifier: MPL-2.0

#include <algorithm>
#include <chrono>
#include <cmath>
#include <limits>
#include <numeric>
#include <vector>

#include "GradientBenchmark.h"


namespace mohidng::benchmarks::gradient {

namespace {

using Clock = std::chrono::steady_clock;


// Executes the measured operation repeatedly inside one timing block.
//
// Grouping very short operations into a block reduces the influence of timer
// resolution and timing-call overhead.
//
// The return values produced by the measured operation are accumulated and
// propagated to the benchmark checksum.
template <class Kernel>
Real RunBlock(
    Kernel&& kernel,
    Size block_size) {

  Real accumulated = 0.0;

  for (Size i = 0;
       i < block_size;
       ++i) {

    accumulated += kernel();
  }

  return accumulated;
}


// Selects a block size large enough to produce a measurable execution interval.
//
// The block starts with one operation and increases by one order of magnitude
// until either:
//
//   - the measured block reaches target_block_seconds; or
//   - max_block_size is reached.
//
// Work performed during this calibration step contributes to the checksum but
// is not included in the statistical samples.
template <class Kernel>
Size ChooseBlockSize(
    Kernel&& kernel,
    const TimingSettings& settings,
    Real& checksum) {

  Size block_size = 1;

  while (true) {

    const auto start =
        Clock::now();

    checksum +=
        RunBlock(
            kernel,
            block_size);

    const Real elapsed_seconds =
        static_cast<Real>(
            std::chrono::duration<double>(
                Clock::now() - start)
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


// Returns a percentile from a sorted sample.
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


// Updates all statistical quantities associated with the collected timing
// samples.
//
// Every entry in measurements represents the estimated execution time of one
// invocation of the measured operation, not the duration of the complete
// timing block.
void UpdateStatistics(
    TimingResult& result,
    const std::vector<Real>& measurements) {

  result.n_measurements =
      static_cast<Size>(
          measurements.size());

  if (measurements.empty()) {
    return;
  }

  const Real sum =
      std::accumulate(
          measurements.begin(),
          measurements.end(),
          Real{0.0});

  result.mean_time_s =
      sum /
      static_cast<Real>(
          measurements.size());

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

  std::vector<Real> sorted =
      measurements;

  std::sort(
      sorted.begin(),
      sorted.end());

  result.median_time_s =
      Percentile(
          sorted,
          0.5);

  result.iqr_time_s =
      Percentile(
          sorted,
          0.75) -
      Percentile(
          sorted,
          0.25);

  if (result.mean_time_s > 0.0) {

    result.coefficient_of_variation =
        result.std_time_s /
        result.mean_time_s;
  } else {

    result.coefficient_of_variation =
        0.0;
  }

  // The present benchmark uses the normal approximation for the 95%
  // confidence interval.
  //
  // Statistical sampling begins with at least 30 measurements, for which this
  // approximation is adequate for the current preparatory benchmark.
  constexpr Real normal_critical_95 =
      static_cast<Real>(
          1.959963984540054);

  result.ci95_half_width_s =
      normal_critical_95 *
      result.std_time_s /
      std::sqrt(
          static_cast<Real>(
              measurements.size()));

  if (result.mean_time_s > 0.0) {

    result.ci95_relative =
        result.ci95_half_width_s /
        result.mean_time_s;
  } else {

    result.ci95_relative =
        std::numeric_limits<Real>::infinity();
  }
}


// Performs the complete timing procedure for one benchmark operation.
//
// The procedure consists of:
//
//   1. automatic block-size calibration;
//   2. warm-up blocks;
//   3. repeated timed blocks;
//   4. conversion of each block time to time per operation;
//   5. statistical convergence evaluation.
//
// Sampling stops when the relative 95% confidence interval reaches the target
// or when max_measurements is reached.
template <class Kernel>
TimingResult BenchmarkKernel(
    Kernel&& kernel,
    const TimingSettings& settings) {

  TimingResult result;

  Real checksum = 0.0;

  result.block_size =
      ChooseBlockSize(
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
        RunBlock(
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
        Clock::now();

    checksum +=
        RunBlock(
            kernel,
            result.block_size);

    const Real elapsed_seconds =
        static_cast<Real>(
            std::chrono::duration<double>(
                Clock::now() - start)
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

      UpdateStatistics(
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

    UpdateStatistics(
        result,
        measurements);

    result.status =
        "max_measurements";
  }

  result.checksum =
      checksum;

  return result;
}


// Produces a compact checksum from a reconstructed gradient field.
//
// The checksum is not a numerical validation metric. Its purpose is to make
// the result of the measured computation observable to the benchmark so that
// the reconstruction work cannot be discarded as irrelevant.
[[nodiscard]] Real GradientChecksum(
    const Vector2Field& gradient) {

  if (gradient.Size() == 0) {
    return 0.0;
  }

  const Size middle =
      static_cast<Size>(
          gradient.Size() / 2);

  const Size last =
      static_cast<Size>(
          gradient.Size() - 1);

  return
      gradient[0].x +
      gradient[middle].y +
      gradient[last].x -
      gradient[0].y;
}

}  // namespace


// Measures one complete traversal of cell-neighbour connectivity.
//
// One measured operation consists of:
//
//   - visiting every cell;
//   - calling MeshView::CellNeighbours() once for that cell;
//   - accumulating the number of returned neighbours.
//
// This provides an isolated measurement of the connectivity access required by
// gradient-workspace preparation.
TimingResult BenchmarkNeighbourAccess(
    const MeshView& mesh,
    const TimingSettings& settings) {

  const auto kernel =
      [&mesh]() -> Real {

    Size total_neighbours = 0;

    for (const auto& cell :
         mesh.Cells()) {

      const auto neighbours =
          mesh.CellNeighbours(
              cell.id);

      total_neighbours +=
          static_cast<Size>(
              neighbours.size());
    }

    return static_cast<Real>(
        total_neighbours);
  };

  return BenchmarkKernel(
      kernel,
      settings);
}


// Measures construction of one complete CellGradientWorkspace.
//
// Every invocation constructs a fresh workspace from the mesh and gradient
// options. No previously prepared workspace is reused inside the timed
// operation.
//
// Diagnostics are read from the resulting workspace and included in the
// checksum so that the completed construction has an observable result.
TimingResult BenchmarkWorkspaceConstruction(
    const MeshView& mesh,
    const GradientOptions& options,
    const TimingSettings& settings) {

  const auto kernel =
      [&mesh,
       &options]() -> Real {

    const CellGradientWorkspace workspace =
        BuildCellGradientWorkspace(
            mesh,
            options);

    const CellGradientDiagnostics diagnostics =
        workspace.Diagnostics();

    return
        static_cast<Real>(
            diagnostics.coefficient_count) +
        static_cast<Real>(
            diagnostics.well_conditioned_count);
  };

  return BenchmarkKernel(
      kernel,
      settings);
}


// Measures application of a previously prepared CellGradientWorkspace.
//
// The workspace itself is supplied by the caller and therefore its
// construction is outside the measured operation.
//
// Each invocation reconstructs the gradient of the supplied scalar field and
// returns a compact checksum derived from the resulting vector field.
TimingResult BenchmarkGradientApplication(
    const CellGradientWorkspace& workspace,
    const ScalarField& field,
    const TimingSettings& settings) {

  const auto kernel =
      [&workspace,
       &field]() -> Real {

    const Vector2Field gradient =
        ComputeCellGradient(
            workspace,
            field);

    return GradientChecksum(
        gradient);
  };

  return BenchmarkKernel(
      kernel,
      settings);
}

}  // namespace mohidng::benchmarks::gradient