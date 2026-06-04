#pragma once

#include <filesystem>

#include <MohidNG/Mesh/MeshView.h>


namespace mohidng {

inline constexpr const char* kVoronoiMeshPackageSchemaVersion = "mohidng.voronoi_mesh_package/0.1";

// Reads a validated Voronoi mesh package produced by VoronoiMeshMaker.
// Mohid-NG consumes the package; it does not generate, remesh, adapt or
// reconstruct mesh connectivity.
[[nodiscard]] MeshView ReadVoronoiMeshPackage(const std::filesystem::path& path);

}  // namespace mohidng
