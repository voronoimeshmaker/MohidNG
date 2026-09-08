// SPDX-License-Identifier: MPL-2.0
#pragma once

#include <filesystem>
#include <string>

#include <MohidNG/Core/Types.h>
#include <MohidNG/Fields/Field.h>
#include <MohidNG/Mesh/MeshView.h>
#include <MohidNG/Numerics/Gradient/Gradient.h>


namespace mohidng::benchmarks::gradient {

// =============================================================================
// Benchmark configuration
// =============================================================================

// Defines the input and output locations used by the gradient benchmark.
//
// The benchmark is self-contained. Its mesh is stored in:
//
//   data/mesh.h5
//
// Results are written to:
//
//   results/
//
// No command-line arguments are required.
struct BenchmarkConfig {
  std::filesystem::path mesh_file;
  std::filesystem::path output_directory;
};


// Creates the fixed configuration used by this benchmark.
//
// Paths are resolved relative to the benchmark source directory so that the
// program does not depend on the process working directory.
[[nodiscard]] BenchmarkConfig MakeBenchmarkConfig();


// =============================================================================
// Timing
// =============================================================================

// Controls the statistical measurement procedure.
//
// Each measured operation is executed repeatedly. Short operations are grouped
// into blocks so that timer resolution does not dominate the measurement.
//
// Warm-up executions are performed before samples are collected.
//
// Sampling stops when either:
//
//   - the relative 95% confidence interval reaches the requested target; or
//   - the maximum number of measurements is reached.
//
// The benchmark is executed serially.
struct TimingSettings {
  Size warmup = 5;
  Size min_measurements = 30;
  Size max_measurements = 500;
  Size max_block_size = 10000;

  Real target_block_seconds = 0.1;
  Real target_ci95_relative = 0.02;
};


// Stores the statistical result of one measured operation.
//
// All reported times correspond to one execution of the measured operation,
// even when several executions are grouped into one timing block.
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


// Measures access to the neighbour lists of all cells.
//
// For one execution, CellNeighbours() is called once for every cell in the
// mesh. The returned neighbour counts are accumulated into a checksum.
//
// This measurement isolates the mesh-connectivity operation used during
// gradient-workspace preparation.
[[nodiscard]] TimingResult BenchmarkNeighbourAccess(
    const MeshView& mesh,
    const TimingSettings& settings);


// Measures construction of the gradient workspace.
//
// BuildCellGradientWorkspace() prepares the reconstruction stencil and its
// coefficients for every mesh cell.
//
// Workspace construction is measured separately because the resulting
// workspace can be reused for subsequent gradient evaluations while the mesh
// and reconstruction configuration remain unchanged.
[[nodiscard]] TimingResult BenchmarkWorkspaceConstruction(
    const MeshView& mesh,
    const GradientOptions& options,
    const TimingSettings& settings);


// Measures application of an already prepared gradient workspace.
//
// Workspace construction is not included in this measurement.
//
// This isolates the cost of applying the reconstruction coefficients to a
// scalar field.
[[nodiscard]] TimingResult BenchmarkGradientApplication(
    const CellGradientWorkspace& workspace,
    const ScalarField& field,
    const TimingSettings& settings);


// =============================================================================
// Error norms
// =============================================================================

// Stores error norms for a reconstructed gradient field.
//
// For each cell, the local error is the Euclidean norm of the difference
// between the numerical and exact gradient.
//
// The reported quantities are:
//
//   L1   arithmetic mean of the local errors
//
//   L2   root-mean-square of the local errors
//
//   Linf maximum local error
struct ErrorNorms {
  Real l1 = 0.0;
  Real l2 = 0.0;
  Real linf = 0.0;

  Size count = 0;
};


// Computes error norms for the constant-field reconstruction.
//
// The scalar field is:
//
//   phi(x,y) = 3
//
// and therefore:
//
//   grad(phi) = (0,0)
[[nodiscard]] ErrorNorms ConstantGradientError(
    const Vector2Field& gradient);


// Computes error norms for the linear-field reconstruction.
//
// The scalar field is:
//
//   phi(x,y) = 2x - 3y + 1
//
// and therefore:
//
//   grad(phi) = (2,-3)
[[nodiscard]] ErrorNorms LinearGradientError(
    const Vector2Field& gradient);


// =============================================================================
// Controlled fields
// =============================================================================

// Creates the constant cell-centred scalar field:
//
//   phi(x,y) = 3
//
// This field provides a basic consistency check because its exact gradient
// vanishes everywhere.
[[nodiscard]] ScalarField MakeConstantField(
    const MeshView& mesh);


// Creates the linear cell-centred scalar field:
//
//   phi(x,y) = 2x - 3y + 1
//
// Its exact gradient is spatially uniform:
//
//   grad(phi) = (2,-3)
//
// A linear field provides a controlled test of first-order gradient
// reconstruction.
[[nodiscard]] ScalarField MakeLinearField(
    const MeshView& mesh);


// =============================================================================
// Output
// =============================================================================

// Prints benchmark identification, input paths and mesh dimensions.
void PrintBenchmarkHeader(
    const BenchmarkConfig& config,
    const MeshView& mesh);


// Prints information about the prepared gradient workspace.
void PrintWorkspaceInformation(
    const CellGradientDiagnostics& diagnostics);


// Prints the statistical result of one timed operation.
void PrintTimingResult(
    const std::string& name,
    const TimingResult& timing);


// Prints the numerical error norms of one controlled field.
void PrintErrorResult(
    const std::string& name,
    const ErrorNorms& error);


// Writes all timing measurements to a CSV file.
//
// The output includes:
//
//   - measured phase;
//   - gradient method;
//   - weighting policy;
//   - mesh dimensions;
//   - stencil size;
//   - statistical timing quantities;
//   - checksum;
//   - convergence status.
void WriteTimingResults(
    const std::filesystem::path& file,
    const CellGradientDiagnostics& diagnostics,
    Size face_count,
    const TimingResult& neighbour_timing,
    const TimingResult& workspace_timing,
    const TimingResult& gradient_timing);


// Writes numerical gradient errors to a CSV file.
void WriteErrorResults(
    const std::filesystem::path& file,
    const ErrorNorms& constant_error,
    const ErrorNorms& linear_error);


// =============================================================================
// Benchmark execution
// =============================================================================

// Runs the complete gradient reconstruction preparatory benchmark.
//
// Execution sequence:
//
//   1. configure benchmark paths;
//   2. read data/mesh.h5;
//   3. validate the mesh;
//   4. configure gradient reconstruction;
//   5. measure cell-neighbour access;
//   6. measure gradient-workspace construction;
//   7. build the controlled scalar fields;
//   8. measure gradient application;
//   9. evaluate numerical errors;
//  10. write CSV results;
//  11. print the final report.
//
// The program is a preparatory benchmark used to develop capabilities needed
// by future scientific cases.
//
// It is not an API example and it is not itself a scientific application case.
//
// Execution is currently serial.
int RunGradientBenchmark();

}  // namespace mohidng::benchmarks::gradient