// SPDX-License-Identifier: MPL-2.0

// MOHID-NG gradient preparatory benchmark.
//
// This file contains only the executable entry point.
//
// The numerical experiment, timing procedure, error evaluation, input-data
// handling, and result generation are implemented in the benchmark modules
// declared through GradientBenchmark.h.
//
// The separation is intentional:
//
//   bench_gradient.cpp
//     Provides the program entry point and top-level exception handling.
//
//   src/GradientBenchmark.cpp
//     Coordinates the complete benchmark workflow.
//
//   src/GradientFields.cpp
//     Defines the controlled scalar fields used for numerical verification.
//
//   src/ErrorNorms.cpp
//     Computes the numerical error norms.
//
//   src/Timing.cpp
//     Implements the timing and statistical measurement procedure.
//
//   src/Output.cpp
//     Produces terminal and persistent benchmark output.
//
// The benchmark uses fixed input data stored in:
//
//   data/mesh.h5
//
// No command-line arguments are required.
//
// The benchmark is a preparatory program used to develop and verify numerical
// capabilities that will later be required by MOHID-NG scientific cases.
//
// It is not an API example and it is not itself a complete scientific case.
//
// Execution is currently serial.

#include <exception>
#include <iostream>

#include "include/GradientBenchmark.h"


// Starts the gradient benchmark.
//
// All benchmark configuration and execution logic remain outside main().
// This keeps the executable entry point independent of the numerical and
// experimental implementation.
//
// Any exception that reaches this level represents an unrecoverable benchmark
// failure. The diagnostic message is written to the standard error stream and
// a non-zero exit status is returned.
int main() {

  try {

    return
        mohidng::benchmarks::gradient::
            RunGradientBenchmark();

  } catch (const std::exception& exception) {

    std::cerr
        << "Gradient benchmark failed: "
        << exception.what()
        << '\n';

    return 1;
  }
}