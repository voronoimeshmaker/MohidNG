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

  return
      std::filesystem::path(__FILE__)
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
// Numerical correctness is evaluated independently with analytical scalar
// fields whose exact gradients are known.
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

  const GradientOptions gradient_options{};

  const TimingSettings timing_settings{};


  // ---------------------------------------------------------------------------
  // Cell-neighbour access
  // ---------------------------------------------------------------------------

  const TimingResult neighbour_timing =
      BenchmarkNeighbourAccess(
          mesh,
          timing_settings);


  // ---------------------------------------------------------------------------
  // Gradient-workspace construction
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
  // This workspace is constructed outside the timed region and reused for
  // gradient application and numerical verification.
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
  // Analytical verification cases
  // ---------------------------------------------------------------------------
  //
  // Constant:
  //
  //   phi(x,y) = 3
  //   grad(phi) = (0,0)
  //
  // Linear:
  //
  //   phi(x,y) = 2x - 3y + 1
  //   grad(phi) = (2,-3)
  //
  // The mathematical definition of each case now contains both the scalar
  // value and its exact gradient.
  // ---------------------------------------------------------------------------

  const AnalyticalScalarCase constant_case =
      MakeConstantCase();

  const AnalyticalScalarCase linear_case =
      MakeLinearCase();


  // ---------------------------------------------------------------------------
  // Sample analytical scalar fields
  // ---------------------------------------------------------------------------

  const ScalarField constant_field =
      SampleScalarField(
          mesh,
          constant_case);

  const ScalarField linear_field =
      SampleScalarField(
          mesh,
          linear_case);


  // ---------------------------------------------------------------------------
  // Gradient application
  // ---------------------------------------------------------------------------
  //
  // Only application of the already prepared workspace is measured.
  //
  // The linear field is used as the controlled timing input.
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
  // These operations are executed outside timed regions because their purpose
  // here is numerical verification.
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
  //
  // The same generic error routine is used for every analytical case.
  // ---------------------------------------------------------------------------

  const ErrorNorms constant_error =
      ComputeGradientError(
          mesh,
          constant_gradient,
          constant_case);

  const ErrorNorms linear_error =
      ComputeGradientError(
          mesh,
          linear_gradient,
          linear_case);


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
      constant_case.name,
      constant_error);

  PrintErrorResult(
      linear_case.name,
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