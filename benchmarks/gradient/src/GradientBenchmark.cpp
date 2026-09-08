// SPDX-License-Identifier: MPL-2.0

#include <filesystem>
#include <iostream>
#include <stdexcept>

#include <MohidNG/IO/VoronoiMeshPackageReader.h>

#include "GradientBenchmark.h"


namespace mohidng::benchmarks::gradient {

namespace {

// Returns the root directory of this gradient benchmark.
//
// This source file is located in:
//
//   benchmarks/gradient/src/GradientBenchmark.cpp
//
// Therefore, moving one directory upwards from src/ gives:
//
//   benchmarks/gradient/
//
// Input and output paths are defined relative to this directory so that
// benchmark execution does not depend on the current working directory.
[[nodiscard]] std::filesystem::path BenchmarkDirectory() {

  return std::filesystem::path(__FILE__)
      .parent_path()
      .parent_path();
}

}  // namespace


// Creates the fixed input and output configuration for the benchmark.
//
// Input:
//
//   data/mesh.h5
//
// Output:
//
//   results/
//
// The benchmark intentionally does not receive file names or numerical
// parameters through main(). Its configuration is part of the controlled
// benchmark definition.
BenchmarkConfig MakeBenchmarkConfig() {

  const auto benchmark_directory =
      BenchmarkDirectory();

  BenchmarkConfig config;

  config.mesh_file =
      benchmark_directory /
      "data" /
      "mesh.h5";

  config.output_directory =
      benchmark_directory /
      "results";

  return config;
}


// Runs the complete gradient reconstruction preparatory benchmark.
//
// The benchmark separates three computational stages:
//
//   1. access to cell-neighbour information;
//   2. construction of the gradient workspace;
//   3. application of an already prepared gradient workspace.
//
// This separation is intentional.
//
// Cell-neighbour access is part of the mesh-connectivity infrastructure used
// while preparing reconstruction stencils.
//
// Workspace construction is a preparatory operation that depends on mesh
// geometry and the selected gradient reconstruction configuration.
//
// Gradient application uses an already prepared workspace and therefore
// represents the cost that may be paid repeatedly while the mesh and
// reconstruction configuration remain unchanged.
//
// Numerical correctness is evaluated independently with controlled constant
// and linear scalar fields.
//
// The benchmark is currently executed serially.
int RunGradientBenchmark() {

  // ---------------------------------------------------------------------------
  // Benchmark configuration
  // ---------------------------------------------------------------------------

  const BenchmarkConfig config =
      MakeBenchmarkConfig();

  if (!std::filesystem::exists(
          config.mesh_file)) {

    throw std::runtime_error(
        "Gradient benchmark mesh file was not found: " +
        config.mesh_file.string());
  }

  std::filesystem::create_directories(
      config.output_directory);


  // ---------------------------------------------------------------------------
  // Mesh
  // ---------------------------------------------------------------------------

  const MeshView mesh =
      ReadVoronoiMeshPackage(
          config.mesh_file);

  if (!mesh.IsValid()) {

    throw std::runtime_error(
        "Gradient benchmark received an invalid mesh.");
  }

  PrintBenchmarkHeader(
      config,
      mesh);


  // ---------------------------------------------------------------------------
  // Gradient reconstruction configuration
  // ---------------------------------------------------------------------------
  //
  // GradientOptions currently supplies the default MOHID-NG reconstruction
  // configuration.
  //
  // The selected method and weighting policy are recorded in the benchmark
  // output through CellGradientDiagnostics.
  // ---------------------------------------------------------------------------

  const GradientOptions gradient_options{};

  const TimingSettings timing_settings{};


  // ---------------------------------------------------------------------------
  // Cell-neighbour access
  // ---------------------------------------------------------------------------
  //
  // This measurement exercises the public MeshView neighbour-access operation
  // once for every cell in the mesh.
  //
  // It is measured independently from workspace construction so that changes
  // in mesh-connectivity implementation can be evaluated separately from the
  // remaining gradient preparation work.
  // ---------------------------------------------------------------------------

  const TimingResult neighbour_timing =
      BenchmarkNeighbourAccess(
          mesh,
          timing_settings);


  // ---------------------------------------------------------------------------
  // Gradient-workspace construction
  // ---------------------------------------------------------------------------
  //
  // BuildCellGradientWorkspace() prepares the per-cell reconstruction stencils
  // and coefficients.
  //
  // The complete workspace-construction operation is timed here.
  // ---------------------------------------------------------------------------

  const TimingResult workspace_timing =
      BenchmarkWorkspaceConstruction(
          mesh,
          gradient_options,
          timing_settings);


  // ---------------------------------------------------------------------------
  // Persistent workspace
  // ---------------------------------------------------------------------------
  //
  // A workspace is now constructed outside any timed region.
  //
  // This instance is used for:
  //
  //   - reporting diagnostics;
  //   - measuring repeated gradient application;
  //   - evaluating numerical error.
  //
  // Its construction time is therefore not included in the gradient
  // application measurement.
  // ---------------------------------------------------------------------------

  const CellGradientWorkspace workspace =
      BuildCellGradientWorkspace(
          mesh,
          gradient_options);

  const CellGradientDiagnostics diagnostics =
      workspace.Diagnostics();

  PrintWorkspaceInformation(
      diagnostics);


  // ---------------------------------------------------------------------------
  // Controlled scalar fields
  // ---------------------------------------------------------------------------
  //
  // Two fields are currently evaluated.
  //
  // Constant field:
  //
  //   phi(x,y) = 3
  //
  //   grad(phi) = (0,0)
  //
  // Linear field:
  //
  //   phi(x,y) = 2x - 3y + 1
  //
  //   grad(phi) = (2,-3)
  //
  // The linear field is also used for the gradient-application timing.
  // ---------------------------------------------------------------------------

  const ScalarField constant_field =
      MakeConstantField(
          mesh);

  const ScalarField linear_field =
      MakeLinearField(
          mesh);


  // ---------------------------------------------------------------------------
  // Gradient application
  // ---------------------------------------------------------------------------
  //
  // Only application of the already prepared workspace is measured.
  //
  // Workspace construction is deliberately excluded.
  // ---------------------------------------------------------------------------

  const TimingResult gradient_timing =
      BenchmarkGradientApplication(
          workspace,
          linear_field,
          timing_settings);


  // ---------------------------------------------------------------------------
  // Numerical reconstruction
  // ---------------------------------------------------------------------------
  //
  // These evaluations are performed outside the timed regions. Their purpose
  // is numerical verification rather than performance measurement.
  // ---------------------------------------------------------------------------

  const Vector2Field constant_gradient =
      ComputeCellGradient(
          workspace,
          constant_field);

  const Vector2Field linear_gradient =
      ComputeCellGradient(
          workspace,
          linear_field);


  // ---------------------------------------------------------------------------
  // Numerical errors
  // ---------------------------------------------------------------------------

  const ErrorNorms constant_error =
      ConstantGradientError(
          constant_gradient);

  const ErrorNorms linear_error =
      LinearGradientError(
          linear_gradient);


  // ---------------------------------------------------------------------------
  // Terminal report
  // ---------------------------------------------------------------------------

  std::cout
      << "\nTiming results\n";

  PrintTimingResult(
      "cell neighbour access",
      neighbour_timing);

  PrintTimingResult(
      "gradient workspace construction",
      workspace_timing);

  PrintTimingResult(
      "gradient application",
      gradient_timing);

  std::cout
      << "\nNumerical error results\n";

  PrintErrorResult(
      "constant field",
      constant_error);

  PrintErrorResult(
      "linear field",
      linear_error);


  // ---------------------------------------------------------------------------
  // Persistent results
  // ---------------------------------------------------------------------------

  const std::filesystem::path timing_file =
      config.output_directory /
      "gradient_timing.csv";

  const std::filesystem::path error_file =
      config.output_directory /
      "gradient_errors.csv";

  WriteTimingResults(
      timing_file,
      diagnostics,
      static_cast<Size>(
          mesh.Faces().size()),
      neighbour_timing,
      workspace_timing,
      gradient_timing);

  WriteErrorResults(
      error_file,
      constant_error,
      linear_error);


  // ---------------------------------------------------------------------------
  // Completion
  // ---------------------------------------------------------------------------

  std::cout
      << "\nGradient benchmark completed.\n"
      << "Results directory: "
      << config.output_directory.string()
      << '\n';

  return 0;
}

}  // namespace mohidng::benchmarks::gradient