#include <filesystem>
#include <stdexcept>
#include <string>

#include <MohidNG/Core/Error.h>
#include <MohidNG/IO/VoronoiMeshPackageReader.h>


namespace {

void Check(bool condition, const std::string& message) {
  if (!condition) {
    throw std::runtime_error(message);
  }
}

void ExpectPackageError(const std::filesystem::path& path, const std::string& message) {
  bool caught = false;
  try {
    (void)mohidng::ReadVoronoiMeshPackage(path);
  } catch (const mohidng::MohidNgException&) {
    caught = true;
  }
  Check(caught, message);
}

}  // namespace

int main(int argc, char** argv) {
  Check(argc == 2, "Expected Voronoi mesh package path argument.");
  const std::filesystem::path valid_path(argv[1]);
  const auto fixture_dir = valid_path.parent_path();

  const auto mesh = mohidng::ReadVoronoiMeshPackage(valid_path);
  Check(mesh.Metadata().schema_version == mohidng::kVoronoiMeshPackageSchemaVersion,
        "Unexpected package schema version.");
  Check(mesh.Metadata().producer == "VoronoiMeshMaker", "Unexpected package producer.");
  Check(mesh.Cells().size() == 4, "Unexpected cell count.");
  Check(mesh.Faces().size() == 12, "Unexpected face count.");
  Check(mesh.Nodes().size() == 9, "Unexpected node count.");
  Check(mesh.BoundaryPatches().size() == 4, "Unexpected boundary patch count.");

  ExpectPackageError(fixture_dir / "square_2x2_negative_area.mngpkg.h5",
                     "Negative cell area package was accepted.");
  ExpectPackageError(fixture_dir / "square_2x2_bad_owner.mngpkg.h5",
                     "Out-of-range face owner package was accepted.");
  ExpectPackageError(fixture_dir / "square_2x2.mngmesh",
                     "Plain-text bootstrap mesh was accepted as HDF5.");

  return 0;
}
