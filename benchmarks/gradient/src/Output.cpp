// SPDX-License-Identifier: MPL-2.0

#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>

#include "GradientBenchmark.h"


namespace mohidng::benchmarks::gradient {

namespace {

// Writes the CSV header used for timing results.
//
// Each row represents one measured phase of the benchmark.
void WriteTimingHeader(
    std::ofstream& output) {

  output
      << "phase,"
      << "method,"
      << "weighting,"
      << "n_cells,"
      << "n_faces,"
      << "n_coefficients,"
      << "block_size,"
      << "n_measurements,"
      << "mean_time_s,"
      << "std_time_s,"
      << "median_time_s,"
      << "iqr_time_s,"
      << "min_time_s,"
      << "max_time_s,"
      << "coefficient_of_variation,"
      << "ci95_half_width_s,"
      << "ci95_relative,"
      << "mean_time_per_cell_s,"
      << "checksum,"
      << "status\n";
}


// Writes one timing result to the CSV file.
//
// The reported mean time corresponds to one execution of the measured
// operation, even when the timing infrastructure groups several executions
// into one block.
void WriteTimingRow(
    std::ofstream& output,
    const std::string& phase,
    const CellGradientDiagnostics& diagnostics,
    Size face_count,
    const TimingResult& timing) {

  Real mean_time_per_cell_s = 0.0;

  if (diagnostics.cell_count > 0) {
    mean_time_per_cell_s =
        timing.mean_time_s /
        static_cast<Real>(
            diagnostics.cell_count);
  }

  output
      << phase << ','
      << diagnostics.method << ','
      << diagnostics.weighting << ','
      << diagnostics.cell_count << ','
      << face_count << ','
      << diagnostics.coefficient_count << ','
      << timing.block_size << ','
      << timing.n_measurements << ','
      << timing.mean_time_s << ','
      << timing.std_time_s << ','
      << timing.median_time_s << ','
      << timing.iqr_time_s << ','
      << timing.min_time_s << ','
      << timing.max_time_s << ','
      << timing.coefficient_of_variation << ','
      << timing.ci95_half_width_s << ','
      << timing.ci95_relative << ','
      << mean_time_per_cell_s << ','
      << timing.checksum << ','
      << timing.status
      << '\n';
}


// Writes the CSV header used for numerical error results.
void WriteErrorHeader(
    std::ofstream& output) {

  output
      << "field,"
      << "count,"
      << "l1,"
      << "l2,"
      << "linf\n";
}


// Writes one set of numerical error norms to the CSV file.
void WriteErrorRow(
    std::ofstream& output,
    const std::string& field,
    const ErrorNorms& error) {

  output
      << field << ','
      << error.count << ','
      << error.l1 << ','
      << error.l2 << ','
      << error.linf
      << '\n';
}

}  // namespace


// Prints the benchmark identification and mesh information.
//
// This information is intended to make each execution self-describing in the
// terminal output.
void PrintBenchmarkHeader(
    const BenchmarkConfig& config,
    const MeshView& mesh) {

  std::cout
      << std::setprecision(10)
      << "MOHID-NG gradient benchmark\n"
      << '\n'
      << "Input\n"
      << "  mesh    = "
      << config.mesh_file.string()
      << '\n'
      << "  results = "
      << config.output_directory.string()
      << '\n'
      << '\n'
      << "Mesh information\n"
      << "  cells = "
      << mesh.Cells().size()
      << '\n'
      << "  faces = "
      << mesh.Faces().size()
      << '\n'
      << "  nodes = "
      << mesh.Nodes().size()
      << '\n';
}


// Prints diagnostic information produced during construction of the gradient
// workspace.
//
// These values describe the reconstruction configuration actually used by the
// benchmark and are therefore part of the execution record.
void PrintWorkspaceInformation(
    const CellGradientDiagnostics& diagnostics) {

  std::cout
      << '\n'
      << "Gradient workspace\n"
      << "  method            = "
      << diagnostics.method
      << '\n'
      << "  weighting         = "
      << diagnostics.weighting
      << '\n'
      << "  cells             = "
      << diagnostics.cell_count
      << '\n'
      << "  coefficients      = "
      << diagnostics.coefficient_count
      << '\n'
      << "  well conditioned  = "
      << diagnostics.well_conditioned_count
      << '\n'
      << "  ill conditioned   = "
      << diagnostics.ill_conditioned_count
      << '\n';
}


// Prints the statistical result of one measured operation.
void PrintTimingResult(
    const std::string& name,
    const TimingResult& timing) {

  std::cout
      << '\n'
      << "  "
      << name
      << '\n'
      << "    mean time             = "
      << timing.mean_time_s
      << " s\n"
      << "    standard deviation    = "
      << timing.std_time_s
      << " s\n"
      << "    median                = "
      << timing.median_time_s
      << " s\n"
      << "    interquartile range   = "
      << timing.iqr_time_s
      << " s\n"
      << "    minimum               = "
      << timing.min_time_s
      << " s\n"
      << "    maximum               = "
      << timing.max_time_s
      << " s\n"
      << "    coefficient variation = "
      << timing.coefficient_of_variation
      << '\n'
      << "    CI95 half width       = "
      << timing.ci95_half_width_s
      << " s\n"
      << "    CI95 relative         = "
      << timing.ci95_relative
      << '\n'
      << "    measurements          = "
      << timing.n_measurements
      << '\n'
      << "    block size            = "
      << timing.block_size
      << '\n'
      << "    checksum              = "
      << timing.checksum
      << '\n'
      << "    status                = "
      << timing.status
      << '\n';
}


// Prints the numerical error norms for one controlled scalar field.
void PrintErrorResult(
    const std::string& name,
    const ErrorNorms& error) {

  std::cout
      << '\n'
      << "  "
      << name
      << '\n'
      << "    cells = "
      << error.count
      << '\n'
      << "    L1    = "
      << error.l1
      << '\n'
      << "    L2    = "
      << error.l2
      << '\n'
      << "    Linf  = "
      << error.linf
      << '\n';
}


// Writes the timing results of all measured benchmark phases.
//
// The file is replaced on each execution so that its contents correspond to
// one complete benchmark run.
void WriteTimingResults(
    const std::filesystem::path& file,
    const CellGradientDiagnostics& diagnostics,
    Size face_count,
    const TimingResult& neighbour_timing,
    const TimingResult& workspace_timing,
    const TimingResult& gradient_timing) {

  std::ofstream output(file);

  if (!output) {
    throw std::runtime_error(
        "Could not create gradient timing file: " +
        file.string());
  }

  output
      << std::setprecision(17);

  WriteTimingHeader(
      output);

  WriteTimingRow(
      output,
      "cell_neighbour_access",
      diagnostics,
      face_count,
      neighbour_timing);

  WriteTimingRow(
      output,
      "gradient_workspace_construction",
      diagnostics,
      face_count,
      workspace_timing);

  WriteTimingRow(
      output,
      "gradient_application",
      diagnostics,
      face_count,
      gradient_timing);
}


// Writes numerical error norms for the controlled scalar fields.
//
// The file is replaced on each execution so that it corresponds to the current
// benchmark run.
void WriteErrorResults(
    const std::filesystem::path& file,
    const ErrorNorms& constant_error,
    const ErrorNorms& linear_error) {

  std::ofstream output(file);

  if (!output) {
    throw std::runtime_error(
        "Could not create gradient error file: " +
        file.string());
  }

  output
      << std::setprecision(17);

  WriteErrorHeader(
      output);

  WriteErrorRow(
      output,
      "constant",
      constant_error);

  WriteErrorRow(
      output,
      "linear",
      linear_error);
}

}  // namespace mohidng::benchmarks::gradient