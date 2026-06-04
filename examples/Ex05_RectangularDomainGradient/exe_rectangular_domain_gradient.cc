// =============================================================================
// Example: build a rectangular 2D domain from YAML and compute a gradient.
//
// Purpose:
// Demonstrates the first YAML-driven input workflow in Mohid-NG. The example
// reads RectangularDomainGradient.ini, creates a rectangular finite-volume
// fixture mesh, defines a linear cell field from input coefficients, builds a
// gradient workspace from input numerical options, and checks the reconstructed
// gradient against the exact analytic value.
//
// Input:
// RectangularDomainGradient.ini, stored in the same directory as this example.
// The file uses YAML syntax even though the example data-file extension is .ini.
//
// How to run from the build directory:
// make run_rectangular_domain_gradient
//
// Direct executable form:
// ./exe_rectangular_domain_gradient <RectangularDomainGradient.ini>
//
// Expected result:
// The program prints the case name, domain size, mesh resolution, field,
// gradient method, weighting policy, and maximum gradient errors. It exits with
// success when both errors remain below the example tolerance.
// =============================================================================

#include <cmath>
#include <cstddef>
#include <filesystem>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include <yaml-cpp/yaml.h>

#include <MohidNG/Core/Constants.h>
#include <MohidNG/Core/Error.h>
#include <MohidNG/Fields/Field.h>
#include <MohidNG/Mesh/MeshView.h>
#include <MohidNG/Numerics/Gradient/Gradient.h>

namespace {

struct RectangularCaseConfig {
  std::string case_name;
  std::string field_name;
  std::size_t nx = 0;
  std::size_t ny = 0;
  mohidng::Real length_x = 0.0;
  mohidng::Real length_y = 0.0;
  mohidng::Real coefficient_x = 0.0;
  mohidng::Real coefficient_y = 0.0;
  mohidng::Real constant = 0.0;
  mohidng::GradientOptions gradient_options;
};

const YAML::Node RequireNode(const YAML::Node& parent, const std::string& key,
                             const std::string& context) {
  const YAML::Node node = parent[key];
  mohidng::Require(static_cast<bool>(node), "case.invalid_yaml", context + "." + key);
  return node;
}

std::string RequireString(const YAML::Node& parent, const std::string& key,
                          const std::string& context) {
  const YAML::Node node = RequireNode(parent, key, context);
  mohidng::Require(node.IsScalar(), "case.invalid_yaml", context + "." + key + " must be scalar");
  return node.as<std::string>();
}

std::size_t RequireSize(const YAML::Node& parent, const std::string& key,
                        const std::string& context) {
  const YAML::Node node = RequireNode(parent, key, context);
  mohidng::Require(node.IsScalar(), "case.invalid_yaml", context + "." + key + " must be scalar");
  return node.as<std::size_t>();
}

mohidng::Real RequireReal(const YAML::Node& parent, const std::string& key,
                          const std::string& context) {
  const YAML::Node node = RequireNode(parent, key, context);
  mohidng::Require(node.IsScalar(), "case.invalid_yaml", context + "." + key + " must be scalar");
  return node.as<mohidng::Real>();
}

mohidng::GradientWeightingPolicy WeightingFromName(const std::string& name) {
  if (name == mohidng::UniformGradientWeighting::Name()) {
    return mohidng::UniformWeightingPolicy();
  }
  if (name == mohidng::InverseDistanceGradientWeighting::Name()) {
    return mohidng::InverseDistanceWeightingPolicy();
  }
  if (name == mohidng::InverseDistanceSquaredGradientWeighting::Name()) {
    return mohidng::InverseDistanceSquaredWeightingPolicy();
  }
  mohidng::Raise(mohidng::Error("case.invalid_yaml", "Unknown gradient weighting: " + name));
}

RectangularCaseConfig ReadCaseConfig(const std::filesystem::path& case_path) {
  const YAML::Node root = YAML::LoadFile(case_path.string());
  const YAML::Node case_node = RequireNode(root, "case", "root");
  const YAML::Node domain_node = RequireNode(root, "domain", "root");
  const YAML::Node mesh_node = RequireNode(root, "mesh", "root");
  const YAML::Node field_node = RequireNode(root, "field", "root");
  const YAML::Node linear_node = RequireNode(field_node, "linear", "field");
  const YAML::Node numerics_node = RequireNode(root, "numerics", "root");
  const YAML::Node gradient_node = RequireNode(numerics_node, "gradient", "numerics");

  mohidng::Require(RequireString(domain_node, "type", "domain") == "rectangle", "case.invalid_yaml",
                   "domain.type must be rectangle");
  mohidng::Require(mohidng::FieldLocation(RequireString(field_node, "location", "field")) ==
                       mohidng::CellFieldLocation(),
                   "case.invalid_yaml", "field.location must be cell");

  RectangularCaseConfig config;
  config.case_name = RequireString(case_node, "name", "case");
  config.field_name = RequireString(field_node, "name", "field");
  config.nx = RequireSize(mesh_node, "cells_x", "mesh");
  config.ny = RequireSize(mesh_node, "cells_y", "mesh");
  config.length_x = RequireReal(domain_node, "length_x", "domain");
  config.length_y = RequireReal(domain_node, "length_y", "domain");
  config.coefficient_x = RequireReal(linear_node, "coefficient_x", "field.linear");
  config.coefficient_y = RequireReal(linear_node, "coefficient_y", "field.linear");
  config.constant = RequireReal(linear_node, "constant", "field.linear");
  config.gradient_options.method = RequireString(gradient_node, "method", "numerics.gradient");
  config.gradient_options.weighting =
      WeightingFromName(RequireString(gradient_node, "weighting", "numerics.gradient"));

  mohidng::Require(config.nx > 0 && config.ny > 0, "case.invalid_yaml",
                   "mesh.cells_x and mesh.cells_y must be positive");
  mohidng::Require(config.length_x > 0.0 && config.length_y > 0.0, "case.invalid_yaml",
                   "domain lengths must be positive");
  return config;
}

mohidng::CellIndex CellId(std::size_t i, std::size_t j, std::size_t nx) {
  return mohidng::CellIndex{static_cast<mohidng::IndexValue>(j * nx + i)};
}

mohidng::MeshView MakeRectangularFixtureMesh(std::size_t nx, std::size_t ny, mohidng::Real length_x,
                                             mohidng::Real length_y) {
  const mohidng::Real dx = length_x / static_cast<mohidng::Real>(nx);
  const mohidng::Real dy = length_y / static_cast<mohidng::Real>(ny);

  std::vector<mohidng::Node2D> nodes;
  nodes.reserve((nx + 1) * (ny + 1));
  for (std::size_t j = 0; j <= ny; ++j) {
    for (std::size_t i = 0; i <= nx; ++i) {
      nodes.push_back(mohidng::Node2D{
          mohidng::NodeIndex{static_cast<mohidng::IndexValue>(j * (nx + 1) + i)},
          mohidng::Vec2{static_cast<mohidng::Real>(i) * dx, static_cast<mohidng::Real>(j) * dy}});
    }
  }

  std::vector<mohidng::Cell2D> cells;
  cells.reserve(nx * ny);
  for (std::size_t j = 0; j < ny; ++j) {
    for (std::size_t i = 0; i < nx; ++i) {
      cells.push_back(mohidng::Cell2D{CellId(i, j, nx),
                                      mohidng::Vec2{(static_cast<mohidng::Real>(i) + 0.5) * dx,
                                                    (static_cast<mohidng::Real>(j) + 0.5) * dy},
                                      dx * dy});
    }
  }

  std::vector<mohidng::Face2D> faces;
  auto next_face = mohidng::IndexValue{0};
  auto add_face = [&](mohidng::CellIndex owner, mohidng::CellIndex neighbour,
                      mohidng::Vec2 centre, mohidng::Vec2 normal, mohidng::Real length,
                      int patch_id) {
    faces.push_back(mohidng::Face2D{mohidng::FaceIndex{next_face++}, owner, neighbour, centre,
                                    normal, length, patch_id});
  };

  constexpr int kLeftPatch = 0;
  constexpr int kRightPatch = 1;
  constexpr int kBottomPatch = 2;
  constexpr int kTopPatch = 3;

  for (std::size_t j = 0; j < ny; ++j) {
    for (std::size_t i = 0; i <= nx; ++i) {
      const mohidng::Vec2 centre{static_cast<mohidng::Real>(i) * dx,
                                 (static_cast<mohidng::Real>(j) + 0.5) * dy};
      if (i == 0) {
        add_face(CellId(0, j, nx), mohidng::CellIndex{}, centre, mohidng::Vec2{-1.0, 0.0}, dy,
                 kLeftPatch);
      } else if (i == nx) {
        add_face(CellId(nx - 1, j, nx), mohidng::CellIndex{}, centre,
                 mohidng::Vec2{1.0, 0.0}, dy, kRightPatch);
      } else {
        add_face(CellId(i - 1, j, nx), CellId(i, j, nx), centre, mohidng::Vec2{1.0, 0.0},
                 dy, -1);
      }
    }
  }

  for (std::size_t j = 0; j <= ny; ++j) {
    for (std::size_t i = 0; i < nx; ++i) {
      const mohidng::Vec2 centre{(static_cast<mohidng::Real>(i) + 0.5) * dx,
                                 static_cast<mohidng::Real>(j) * dy};
      if (j == 0) {
        add_face(CellId(i, 0, nx), mohidng::CellIndex{}, centre, mohidng::Vec2{0.0, -1.0},
                 dx, kBottomPatch);
      } else if (j == ny) {
        add_face(CellId(i, ny - 1, nx), mohidng::CellIndex{}, centre,
                 mohidng::Vec2{0.0, 1.0}, dx, kTopPatch);
      } else {
        add_face(CellId(i, j - 1, nx), CellId(i, j, nx), centre, mohidng::Vec2{0.0, 1.0},
                 dx, -1);
      }
    }
  }

  std::vector<mohidng::BoundaryPatch> patches{
      mohidng::BoundaryPatch{kLeftPatch, "left", "wall"},
      mohidng::BoundaryPatch{kRightPatch, "right", "wall"},
      mohidng::BoundaryPatch{kBottomPatch, "bottom", "wall"},
      mohidng::BoundaryPatch{kTopPatch, "top", "wall"},
  };

  mohidng::MeshMetadata metadata;
  metadata.schema_version = "mohidng.example.rectangular_fixture/0.1";
  metadata.producer = "Mohid-NG example fixture";
  metadata.source = "YAML-driven rectangular finite-volume fixture";
  metadata.coordinate_reference_system = "local Cartesian";
  metadata.note = "Didactic Block 2 mesh fixture; not a production Voronoi mesh generator.";

  return mohidng::MeshView(std::move(nodes), std::move(cells), std::move(faces),
                           std::move(patches), std::move(metadata));
}

mohidng::Real AnalyticField(const RectangularCaseConfig& config, mohidng::Real x, mohidng::Real y) {
  return config.coefficient_x * x + config.coefficient_y * y + config.constant;
}

}  // namespace

int main(int argc, char** argv) {
  try {
    mohidng::Require(argc == 2, "cli.invalid_arguments",
                     "Usage: exe_rectangular_domain_gradient <RectangularDomainGradient.ini>");

    const std::filesystem::path case_path(argv[1]);
    mohidng::Require(std::filesystem::exists(case_path), "case.file_not_found", case_path.string());
    const auto config = ReadCaseConfig(case_path);

    const auto mesh = MakeRectangularFixtureMesh(config.nx, config.ny, config.length_x,
                                                 config.length_y);
    mohidng::ScalarField phi(config.field_name, mohidng::CellFieldLocation(), mesh.Cells().size());

    for (const auto& cell : mesh.Cells()) {
      phi[static_cast<std::size_t>(cell.id.value)] = AnalyticField(config, cell.centre.x, cell.centre.y);
    }

    const auto workspace = mohidng::BuildCellGradientWorkspace(mesh, config.gradient_options);
    const auto gradient = mohidng::ComputeCellGradient(workspace, phi);

    mohidng::Real max_error_x = 0.0;
    mohidng::Real max_error_y = 0.0;
    for (std::size_t i = 0; i < gradient.Size(); ++i) {
      max_error_x = std::max(max_error_x, std::abs(gradient[i].x - config.coefficient_x));
      max_error_y = std::max(max_error_y, std::abs(gradient[i].y - config.coefficient_y));
    }

    std::cout << "Case: " << config.case_name << std::endl;
    std::cout << "Domain: rectangle [0," << config.length_x << "] x [0," << config.length_y
              << "]" << std::endl;
    std::cout << "Mesh: " << config.nx << " x " << config.ny << " cells" << std::endl;
    std::cout << "Field: " << config.field_name << "(x,y) = " << config.coefficient_x
              << " x + " << config.coefficient_y << " y + " << config.constant << std::endl;
    std::cout << "Gradient method: " << config.gradient_options.method << std::endl;
    std::cout << "Weighting: " << config.gradient_options.weighting.Name() << std::endl;
    std::cout << "Max |dphi/dx - expected|: " << max_error_x << std::endl;
    std::cout << "Max |dphi/dy - expected|: " << max_error_y << std::endl;

    if (max_error_x > mohidng::kDefaultTolerance || max_error_y > mohidng::kDefaultTolerance) {
      std::cerr << "Rectangular domain gradient example failed." << std::endl;
      return 2;
    }

    return 0;
  } catch (const YAML::Exception& exception) {
    try {
      mohidng::Raise(mohidng::Error("case.invalid_yaml", exception.what()));
    } catch (const mohidng::MohidNgException& wrapped) {
      std::cerr << wrapped.what() << std::endl;
      return 1;
    }
  } catch (const mohidng::MohidNgException& exception) {
    std::cerr << exception.what() << std::endl;
    return 1;
  }
}
