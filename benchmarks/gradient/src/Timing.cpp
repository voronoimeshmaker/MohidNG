// SPDX-License-Identifier: MPL-2.0

#include "GradientBenchmark.h"

#include <MohidNG/Numerics/Gradient/Gradient.h>


namespace mohidng::benchmarks::gradient {

namespace {

// Produces a compact checksum from a reconstructed gradient field.
//
// The checksum is not a numerical validation metric.
//
// Its purpose is to keep the result of the measured computation observable to
// the benchmark so that the compiler cannot treat the reconstruction work as
// irrelevant.
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
// The generic timing procedure itself is provided by MeasureBenchmark().
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

    return
        static_cast<Real>(
            total_neighbours);
  };

  return
      MeasureBenchmark(
          kernel,
          settings);
}


// Measures construction of one complete CellGradientWorkspace.
//
// Every invocation constructs a fresh workspace from the mesh and gradient
// options.
//
// No previously prepared workspace is reused inside the timed operation.
//
// Diagnostics are included in the returned checksum so that completion of the
// workspace construction remains observable.
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

  return
      MeasureBenchmark(
          kernel,
          settings);
}


// Measures application of an already prepared CellGradientWorkspace.
//
// Workspace construction is deliberately outside the timed operation.
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

    return
        GradientChecksum(
            gradient);
  };

  return
      MeasureBenchmark(
          kernel,
          settings);
}

}  // namespace mohidng::benchmarks::gradient