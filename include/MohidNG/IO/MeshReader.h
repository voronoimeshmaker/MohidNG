#pragma once

#include <filesystem>

#include "MohidNG/Mesh/MeshView.h"

namespace mohidng {

// Temporary bootstrap reader for a simple text format stored in data/meshes.
// Final Voronoi mesh packages shall be produced by VoronoiMeshMaker and read
// through a stable Mohid-NG mesh package contract.
[[nodiscard]] MeshView ReadBootstrapMesh(const std::filesystem::path& path);

}  // namespace mohidng
