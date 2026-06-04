// =============================================================================
// Example: create a scalar cell field.
//
// Purpose:
// Shows how to read a mesh and allocate a ScalarField located at cell
// centres. The field is filled with a simple analytic expression based on
// each cell centre, then the minimum and maximum values are printed.
//
// Input:
// A bootstrap mesh file passed as the first command-line argument.
//
// How to run from the build directory:
// make run_cell_field
//
// Direct executable form:
// ./exe_cell_field <mesh-file>
//
// Expected result:
// The program prints the field name and its minimum and maximum values.
// =============================================================================

#include <algorithm>
#include <filesystem>
#include <iostream>

#include <MohidNG/Core/Error.h>
#include <MohidNG/Fields/Field.h>
#include <MohidNG/IO/MeshReader.h>


int main(int argc, char** argv) {
  try {
    mohidng::Require(argc == 2, "cli.invalid_arguments", "Usage: exe_cell_field <mesh-file>");

    const auto mesh = mohidng::ReadBootstrapMesh(std::filesystem::path(argv[1]));
    mohidng::ScalarField elevation("surface_elevation", mohidng::CellFieldLocation(),
                                   mesh.Cells().size());

    for (const auto& cell : mesh.Cells()) {
      elevation[static_cast<std::size_t>(cell.id.value)] = cell.centre.x + 2.0 * cell.centre.y;
    }

    const auto values = elevation.Values();
    const auto [min_it, max_it] = std::minmax_element(values.begin(), values.end());
    std::cout << "Field: " << elevation.Name() << "\n";
    std::cout << "Minimum: " << *min_it << "\n";
    std::cout << "Maximum: " << *max_it << "\n";
    return 0;
  } catch (const mohidng::MohidNgException& exception) {
    std::cerr << exception.what() << "\n";
    return 1;
  }
}
