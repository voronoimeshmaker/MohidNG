#include "MohidNG/IO/VoronoiMeshPackageReader.h"

#include <hdf5.h>

#include <cmath>
#include <cstdint>
#include <filesystem>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "MohidNG/Core/Error.h"
#include "MohidNG/Core/Logger.h"

namespace mohidng {
namespace {

class Hdf5Object {
 public:
  Hdf5Object() = default;
  Hdf5Object(hid_t id, herr_t (*close)(hid_t)) : id_(id), close_(close) {}
  Hdf5Object(const Hdf5Object&) = delete;
  Hdf5Object& operator=(const Hdf5Object&) = delete;
  Hdf5Object(Hdf5Object&& other) noexcept : id_(other.id_), close_(other.close_) {
    other.id_ = -1;
    other.close_ = nullptr;
  }
  Hdf5Object& operator=(Hdf5Object&& other) noexcept {
    if (this != &other) {
      Close();
      id_ = other.id_;
      close_ = other.close_;
      other.id_ = -1;
      other.close_ = nullptr;
    }
    return *this;
  }
  ~Hdf5Object() { Close(); }

  [[nodiscard]] hid_t Id() const { return id_; }
  [[nodiscard]] bool IsValid() const { return id_ >= 0; }

 private:
  void Close() {
    if (id_ >= 0 && close_ != nullptr) {
      close_(id_);
    }
  }

  hid_t id_ = -1;
  herr_t (*close_)(hid_t) = nullptr;
};

[[nodiscard]] Hdf5Object OpenDataset(hid_t file, const char* path) {
  Hdf5Object dataset(H5Dopen2(file, path, H5P_DEFAULT), H5Dclose);
  Require(dataset.IsValid(), "mesh.parse_error", std::string("Missing dataset: ") + path);
  return dataset;
}

[[nodiscard]] std::vector<hsize_t> DatasetDims(hid_t dataset, const char* path) {
  Hdf5Object space(H5Dget_space(dataset), H5Sclose);
  Require(space.IsValid(), "mesh.parse_error", std::string("Cannot inspect dataset: ") + path);
  const int rank = H5Sget_simple_extent_ndims(space.Id());
  Require(rank > 0, "mesh.parse_error", std::string("Invalid dataset rank: ") + path);
  std::vector<hsize_t> dims(static_cast<std::size_t>(rank));
  const int status = H5Sget_simple_extent_dims(space.Id(), dims.data(), nullptr);
  Require(status >= 0, "mesh.parse_error", std::string("Cannot read dataset dimensions: ") + path);
  return dims;
}

[[nodiscard]] std::vector<double> ReadDoubleDataset(hid_t file, const char* path, int expected_rank,
                                                    hsize_t expected_columns = 0) {
  const auto dataset = OpenDataset(file, path);
  const auto dims = DatasetDims(dataset.Id(), path);
  Require(static_cast<int>(dims.size()) == expected_rank, "mesh.parse_error",
          std::string("Unexpected dataset rank: ") + path);
  if (expected_rank == 2 && expected_columns > 0) {
    Require(dims[1] == expected_columns, "mesh.parse_error",
            std::string("Unexpected dataset column count: ") + path);
  }
  std::size_t count = 1;
  for (const hsize_t dim : dims) {
    count *= static_cast<std::size_t>(dim);
  }
  std::vector<double> values(count);
  const herr_t status = H5Dread(dataset.Id(), H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT,
                                values.data());
  Require(status >= 0, "mesh.parse_error", std::string("Cannot read dataset: ") + path);
  return values;
}

[[nodiscard]] std::vector<IndexValue> ReadIndexDataset(hid_t file, const char* path) {
  const auto dataset = OpenDataset(file, path);
  const auto dims = DatasetDims(dataset.Id(), path);
  Require(dims.size() == 1, "mesh.parse_error", std::string("Unexpected dataset rank: ") + path);
  std::vector<std::int64_t> raw(static_cast<std::size_t>(dims[0]));
  const herr_t status = H5Dread(dataset.Id(), H5T_NATIVE_INT64, H5S_ALL, H5S_ALL, H5P_DEFAULT,
                                raw.data());
  Require(status >= 0, "mesh.parse_error", std::string("Cannot read dataset: ") + path);
  return {raw.begin(), raw.end()};
}

[[nodiscard]] std::vector<int> ReadIntDataset(hid_t file, const char* path) {
  const auto dataset = OpenDataset(file, path);
  const auto dims = DatasetDims(dataset.Id(), path);
  Require(dims.size() == 1, "mesh.parse_error", std::string("Unexpected dataset rank: ") + path);
  std::vector<int> values(static_cast<std::size_t>(dims[0]));
  const herr_t status = H5Dread(dataset.Id(), H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT,
                                values.data());
  Require(status >= 0, "mesh.parse_error", std::string("Cannot read dataset: ") + path);
  return values;
}

[[nodiscard]] std::vector<std::string> ReadStringDataset(hid_t file, const char* path) {
  const auto dataset = OpenDataset(file, path);
  const auto dims = DatasetDims(dataset.Id(), path);
  Require(dims.size() == 1, "mesh.parse_error", std::string("Unexpected dataset rank: ") + path);
  Hdf5Object type(H5Dget_type(dataset.Id()), H5Tclose);
  Require(type.IsValid(), "mesh.parse_error", std::string("Cannot inspect string dataset: ") + path);
  const std::size_t width = H5Tget_size(type.Id());
  Require(width > 0, "mesh.parse_error", std::string("Invalid string width: ") + path);

  std::vector<char> buffer(static_cast<std::size_t>(dims[0]) * width, '\0');
  const herr_t status = H5Dread(dataset.Id(), type.Id(), H5S_ALL, H5S_ALL, H5P_DEFAULT,
                                buffer.data());
  Require(status >= 0, "mesh.parse_error", std::string("Cannot read dataset: ") + path);

  std::vector<std::string> values;
  values.reserve(static_cast<std::size_t>(dims[0]));
  for (std::size_t i = 0; i < static_cast<std::size_t>(dims[0]); ++i) {
    std::string value(buffer.data() + i * width, width);
    const auto end = value.find('\0');
    if (end != std::string::npos) {
      value.resize(end);
    }
    while (!value.empty() && value.back() == ' ') {
      value.pop_back();
    }
    values.push_back(std::move(value));
  }
  return values;
}

[[nodiscard]] std::string ReadStringAttribute(hid_t object, const char* name) {
  Require(H5Aexists(object, name) > 0, "mesh.parse_error", std::string("Missing attribute: ") + name);
  Hdf5Object attr(H5Aopen(object, name, H5P_DEFAULT), H5Aclose);
  Hdf5Object type(H5Aget_type(attr.Id()), H5Tclose);
  Require(attr.IsValid() && type.IsValid(), "mesh.parse_error",
          std::string("Cannot open string attribute: ") + name);

  if (H5Tis_variable_str(type.Id()) > 0) {
    char* raw = nullptr;
    const herr_t status = H5Aread(attr.Id(), type.Id(), &raw);
    Require(status >= 0 && raw != nullptr, "mesh.parse_error",
            std::string("Cannot read string attribute: ") + name);
    std::string value(raw);
    H5free_memory(raw);
    return value;
  }

  const std::size_t width = H5Tget_size(type.Id());
  std::vector<char> buffer(width + 1, '\0');
  const herr_t status = H5Aread(attr.Id(), type.Id(), buffer.data());
  Require(status >= 0, "mesh.parse_error", std::string("Cannot read string attribute: ") + name);
  return std::string(buffer.data());
}

[[nodiscard]] int ReadIntAttribute(hid_t object, const char* name) {
  Require(H5Aexists(object, name) > 0, "mesh.parse_error", std::string("Missing attribute: ") + name);
  Hdf5Object attr(H5Aopen(object, name, H5P_DEFAULT), H5Aclose);
  int value = 0;
  const herr_t status = H5Aread(attr.Id(), H5T_NATIVE_INT, &value);
  Require(status >= 0, "mesh.parse_error", std::string("Cannot read integer attribute: ") + name);
  return value;
}

void ValidatePackageMesh(const MeshView& mesh) {
  MOHIDNG_TRACE_SCOPE("ValidatePackageMesh");
  Require(mesh.Metadata().schema_version == kVoronoiMeshPackageSchemaVersion,
          "mesh.parse_error", "Unsupported Voronoi mesh package schema version.");
  Require(mesh.Metadata().producer == "VoronoiMeshMaker", "mesh.parse_error",
          "Voronoi mesh packages must be produced by VoronoiMeshMaker.");
  Require(mesh.Metadata().dimension == 2, "mesh.parse_error", "Only 2-D packages are supported.");

  const auto cells = mesh.Cells();
  const auto faces = mesh.Faces();
  const auto nodes = mesh.Nodes();
  const auto patches = mesh.BoundaryPatches();
  Require(!cells.empty(), "mesh.invalid_connectivity", "Package has no cells.");
  Require(!faces.empty(), "mesh.invalid_connectivity", "Package has no faces.");
  Require(!nodes.empty(), "mesh.invalid_connectivity", "Package has no nodes.");

  for (std::size_t i = 0; i < cells.size(); ++i) {
    const auto& cell = cells[i];
    Require(cell.id.value == static_cast<IndexValue>(i), "mesh.invalid_connectivity",
            "Cell ids must be contiguous and zero-based.");
    Require(std::isfinite(cell.centre.x) && std::isfinite(cell.centre.y) && cell.area > 0.0,
            "mesh.invalid_connectivity", "Invalid cell geometry.");
  }

  for (std::size_t i = 0; i < nodes.size(); ++i) {
    const auto& node = nodes[i];
    Require(node.id.value == static_cast<IndexValue>(i), "mesh.invalid_connectivity",
            "Node ids must be contiguous and zero-based.");
    Require(std::isfinite(node.point.x) && std::isfinite(node.point.y), "mesh.invalid_connectivity",
            "Invalid node coordinates.");
  }

  for (const auto& face : faces) {
    Require(face.id.value >= 0, "mesh.invalid_connectivity", "Invalid face id.");
    Require(face.owner.value >= 0 && face.owner.value < static_cast<IndexValue>(cells.size()),
            "mesh.invalid_connectivity", "Face owner out of range.");
    if (IsValid(face.neighbour)) {
      Require(face.neighbour.value < static_cast<IndexValue>(cells.size()), "mesh.invalid_connectivity",
              "Face neighbour out of range.");
      Require(face.patch_id < 0, "mesh.invalid_connectivity",
              "Interior faces must not reference a boundary patch.");
    } else {
      Require(face.patch_id >= 0, "mesh.invalid_connectivity",
              "Boundary faces must reference a boundary patch.");
      const auto patch_exists = [patch_id = face.patch_id](std::span<const BoundaryPatch> ps) {
        for (const auto& patch : ps) {
          if (patch.id == patch_id) {
            return true;
          }
        }
        return false;
      }(patches);
      Require(patch_exists, "mesh.invalid_connectivity", "Boundary face references unknown patch.");
    }
    const double normal_norm = std::hypot(face.unit_normal.x, face.unit_normal.y);
    Require(std::isfinite(face.centre.x) && std::isfinite(face.centre.y) && face.length > 0.0,
            "mesh.invalid_connectivity", "Invalid face geometry.");
    Require(std::abs(normal_norm - 1.0) < 1.0e-8, "mesh.invalid_connectivity",
            "Face normal must be unit length.");
  }

  Require(mesh.IsValid(), "mesh.invalid_connectivity", "MeshView validation failed.");
}

}  // namespace

MeshView ReadVoronoiMeshPackage(const std::filesystem::path& path) {
  MOHIDNG_TRACE_SCOPE("ReadVoronoiMeshPackage");
  H5Eset_auto2(H5E_DEFAULT, nullptr, nullptr);
  Hdf5Object file(H5Fopen(path.string().c_str(), H5F_ACC_RDONLY, H5P_DEFAULT), H5Fclose);
  Require(file.IsValid(), "mesh.file_not_found", path.string());

  MeshMetadata metadata;
  metadata.dimension = ReadIntAttribute(file.Id(), "dimension");
  metadata.schema_version = ReadStringAttribute(file.Id(), "schema_version");
  metadata.producer = ReadStringAttribute(file.Id(), "producer");
  metadata.coordinate_reference_system = ReadStringAttribute(file.Id(), "coordinate_reference_system");
  metadata.source = path.string();
  metadata.note = ReadStringAttribute(file.Id(), "note");

  const auto node_ids = ReadIndexDataset(file.Id(), "/nodes/id");
  const auto node_xy = ReadDoubleDataset(file.Id(), "/nodes/xy", 2, 2);
  Require(node_xy.size() == node_ids.size() * 2, "mesh.parse_error", "Node coordinate count mismatch.");
  std::vector<Node2D> nodes(node_ids.size());
  for (std::size_t i = 0; i < node_ids.size(); ++i) {
    nodes[i].id = NodeIndex{node_ids[i]};
    nodes[i].point = Vec2{node_xy[2 * i], node_xy[2 * i + 1]};
  }

  const auto cell_ids = ReadIndexDataset(file.Id(), "/cells/id");
  const auto cell_centres = ReadDoubleDataset(file.Id(), "/cells/centre", 2, 2);
  const auto cell_areas = ReadDoubleDataset(file.Id(), "/cells/area", 1);
  Require(cell_centres.size() == cell_ids.size() * 2 && cell_areas.size() == cell_ids.size(),
          "mesh.parse_error", "Cell dataset size mismatch.");
  std::vector<Cell2D> cells(cell_ids.size());
  for (std::size_t i = 0; i < cell_ids.size(); ++i) {
    cells[i].id = CellIndex{cell_ids[i]};
    cells[i].centre = Vec2{cell_centres[2 * i], cell_centres[2 * i + 1]};
    cells[i].area = cell_areas[i];
  }

  const auto face_ids = ReadIndexDataset(file.Id(), "/faces/id");
  const auto owners = ReadIndexDataset(file.Id(), "/faces/owner");
  const auto neighbours = ReadIndexDataset(file.Id(), "/faces/neighbour");
  const auto face_centres = ReadDoubleDataset(file.Id(), "/faces/centre", 2, 2);
  const auto face_normals = ReadDoubleDataset(file.Id(), "/faces/unit_normal", 2, 2);
  const auto face_lengths = ReadDoubleDataset(file.Id(), "/faces/length", 1);
  const auto patch_ids = ReadIntDataset(file.Id(), "/faces/patch_id");
  const std::size_t face_count = face_ids.size();
  Require(owners.size() == face_count && neighbours.size() == face_count &&
              face_centres.size() == face_count * 2 && face_normals.size() == face_count * 2 &&
              face_lengths.size() == face_count && patch_ids.size() == face_count,
          "mesh.parse_error", "Face dataset size mismatch.");
  std::vector<Face2D> faces(face_count);
  for (std::size_t i = 0; i < face_count; ++i) {
    faces[i].id = FaceIndex{face_ids[i]};
    faces[i].owner = CellIndex{owners[i]};
    faces[i].neighbour = CellIndex{neighbours[i]};
    faces[i].centre = Vec2{face_centres[2 * i], face_centres[2 * i + 1]};
    faces[i].unit_normal = Vec2{face_normals[2 * i], face_normals[2 * i + 1]};
    faces[i].length = face_lengths[i];
    faces[i].patch_id = patch_ids[i];
  }

  const auto boundary_patch_ids = ReadIntDataset(file.Id(), "/boundary_patches/id");
  const auto boundary_patch_names = ReadStringDataset(file.Id(), "/boundary_patches/name");
  const auto boundary_patch_types = ReadStringDataset(file.Id(), "/boundary_patches/type");
  Require(boundary_patch_names.size() == boundary_patch_ids.size() &&
              boundary_patch_types.size() == boundary_patch_ids.size(),
          "mesh.parse_error", "Boundary patch dataset size mismatch.");
  std::vector<BoundaryPatch> patches(boundary_patch_ids.size());
  for (std::size_t i = 0; i < boundary_patch_ids.size(); ++i) {
    patches[i].id = boundary_patch_ids[i];
    patches[i].name = boundary_patch_names[i];
    patches[i].type = boundary_patch_types[i];
  }

  MeshView mesh(std::move(nodes), std::move(cells), std::move(faces), std::move(patches),
                std::move(metadata));
  ValidatePackageMesh(mesh);
  return mesh;
}

}  // namespace mohidng
