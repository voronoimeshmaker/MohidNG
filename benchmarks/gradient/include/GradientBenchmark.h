// SPDX-License-Identifier: MPL-2.0
#pragma once

#include <filesystem>
#include <string>

#include "BenchmarkTiming.h"

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
// Analytical scalar cases
// =============================================================================

// Function type used to evaluate an analytical scalar field.
//
// The arguments are the Cartesian coordinates of the evaluation point.
using ScalarValueFunction =
    Real (*)(Real x, Real y);


// Function type used to evaluate the exact analytical gradient.
//
// The returned Vec2 contains:
//
//   x = d(phi)/dx
//   y = d(phi)/dy
using GradientValueFunction =
    Vec2 (*)(Real x, Real y);


// Describes one analytical scalar-field case used to verify a numerical
// gradient reconstruction.
//
// A case contains:
//
//   - a human-readable name;
//   - the analytical scalar value phi(x,y);
//   - the exact analytical gradient grad(phi)(x,y).
//
// The numerical verification infrastructure is therefore independent of the
// particular analytical expression being tested.
struct AnalyticalScalarCase {
  std::string name;

  ScalarValueFunction value = nullptr;
  GradientValueFunction gradient = nullptr;
};


// Returns the constant analytical case:
//
//   phi(x,y) = 3
//
//   grad(phi) = (0,0)
[[nodiscard]] AnalyticalScalarCase MakeConstantCase();


// Returns the linear analytical case:
//
//   phi(x,y) = 2x - 3y + 1
//
//   grad(phi) = (2,-3)
[[nodiscard]] AnalyticalScalarCase MakeLinearCase();


// Samples an analytical scalar case at all finite-volume cell centres.
//
// The returned ScalarField follows the cell-index storage convention used by
// the MOHID-NG field infrastructure.
[[nodiscard]] ScalarField SampleScalarField(
    const MeshView& mesh,
    const AnalyticalScalarCase& analytical_case);


// =============================================================================
// Timing
// =============================================================================
//
// TimingSettings, TimingResult and MeasureBenchmark() are provided by:
//
//   benchmarks/common/BenchmarkTiming.h
//
// They belong to:
//
//   mohidng::benchmarks
//
// because the timing infrastructure is shared by all MOHID-NG benchmarks.
// =============================================================================


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


// Computes the reconstruction error against an arbitrary analytical scalar
// case.
//
// The exact gradient is evaluated at each finite-volume cell centre.
[[nodiscard]] ErrorNorms ComputeGradientError(
    const MeshView& mesh,
    const Vector2Field& numerical_gradient,
    const AnalyticalScalarCase& analytical_case);


// =============================================================================
// Transitional compatibility interface
// =============================================================================
//
// The functions below are retained temporarily while GradientBenchmark.cpp is
// migrated to AnalyticalScalarCase.
//
// Once all callers use MakeConstantCase(), MakeLinearCase(),
// SampleScalarField() and ComputeGradientError(), these compatibility wrappers
// will be removed.
// =============================================================================


// Creates the constant cell-centred scalar field:
//
//   phi(x,y) = 3
[[nodiscard]] ScalarField MakeConstantField(
    const MeshView& mesh);


// Creates the linear cell-centred scalar field:
//
//   phi(x,y) = 2x - 3y + 1
[[nodiscard]] ScalarField MakeLinearField(
    const MeshView& mesh);


// Computes error norms for the constant-field reconstruction.
[[nodiscard]] ErrorNorms ConstantGradientError(
    const Vector2Field& gradient);


// Computes error norms for the linear-field reconstruction.
[[nodiscard]] ErrorNorms LinearGradientError(
    const Vector2Field& gradient);


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
//   7. build analytical scalar fields;
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