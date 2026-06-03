#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "MohidNG/Core/Index.h"

namespace mohidng {

struct Vec2 {
  double x = 0.0;
  double y = 0.0;
};

struct Node2D {
  NodeIndex id{};
  Vec2 point{};
};

struct Cell2D {
  CellIndex id{};
  Vec2 centre{};
  double area = 0.0;
};

struct Face2D {
  FaceIndex id{};
  CellIndex owner{};
  CellIndex neighbour{};
  Vec2 centre{};
  Vec2 unit_normal{};
  double length = 0.0;
  int patch_id = -1;
};

struct BoundaryPatch {
  int id = -1;
  std::string name;
  std::string type;
};

struct MeshMetadata {
  int dimension = 2;
  std::string source = "bootstrap plain-text mesh";
  std::string coordinate_reference_system = "local Cartesian";
  std::string note = "Temporary bootstrap format. Final mesh packages are owned by VoronoiMeshMaker.";
};

}  // namespace mohidng
