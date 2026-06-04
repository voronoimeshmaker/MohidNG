// =============================================================================
// Example: reconstruct the gradient of a linear cell field.
//
// Purpose:
// Demonstrates the cell-gradient API on a mesh read from disk. The example
// defines phi(x,y) = x + 2 y at cell centres, computes the reconstructed
// gradient, prints one gradient per cell, and checks that the result matches
// the analytic gradient (1, 2) within a tight tolerance.
//
// Input:
// A bootstrap mesh file passed as the first command-line argument.
//
// How to run from the build directory:
// make run_gradient_reconstruction
//
// Direct executable form:
// ./exe_gradient_reconstruction <mesh-file>
//
// Expected result:
// The program prints gradients close to (1, 2) for all cells and exits with
// success. It returns a non-zero code if the reconstruction check fails.
// =============================================================================

#include <cmath>
#include <filesystem>
#include <iostream>

#include <MohidNG/Core/Constants.h>
#include <MohidNG/Core/Error.h>
#include <MohidNG/Fields/Field.h>
#include <MohidNG/IO/MeshReader.h>
#include <MohidNG/Numerics/Gradient/Gradient.h>


int main(int argc, char** argv) {
  try {
    mohidng::Require(argc == 2, "cli.invalid_arguments",
                     "Usage: exe_gradient_reconstruction <mesh-file>");

    const auto mesh = mohidng::ReadBootstrapMesh(std::filesystem::path(argv[1]));
    mohidng::ScalarField phi("phi", mohidng::CellFieldLocation(), mesh.Cells().size());

    for (const auto& cell : mesh.Cells()) {
      phi[static_cast<std::size_t>(cell.id.value)] = cell.centre.x + 2.0 * cell.centre.y;
    }

    const auto grad_phi = mohidng::ComputeCellGradient(mesh, phi);
    for (std::size_t i = 0; i < grad_phi.Size(); ++i) {
      const auto gradient = grad_phi[i];
      std::cout << "cell " << i << " gradient = (" << gradient.x << ", " << gradient.y << ")\n";
      if (std::abs(gradient.x - 1.0) > mohidng::kDefaultTolerance || std::abs(gradient.y - 2.0) > mohidng::kDefaultTolerance) {
        std::cerr << "Gradient reconstruction failed for linear field.\n";
        return 2;
      }
    }

    return 0;
  } catch (const mohidng::MohidNgException& exception) {
    std::cerr << exception.what() << "\n";
    return 1;
  }
}
