#include <string>

#include <fstream>

#include <MohidNG/Core/Error.h>
#include <MohidNG/Core/Logger.h>
#include <MohidNG/IO/MeshReader.h>


namespace mohidng {
namespace {

void ExpectToken(std::istream& stream, const std::string& expected) {
  MOHIDNG_TRACE_SCOPE("ExpectToken");
  std::string token;
  stream >> token;
  Require(token == expected, "mesh.parse_error",
          "Unexpected token while reading bootstrap mesh. Expected: " + expected);
}

}  // namespace

MeshView ReadBootstrapMesh(const std::filesystem::path& path) {
  MOHIDNG_TRACE_SCOPE("ReadBootstrapMesh");
  std::ifstream input(path);
  Require(input.good(), "mesh.file_not_found", path.string());

  ExpectToken(input, "MNGMESH1");

  MeshMetadata metadata;
  std::vector<Node2D> nodes;
  std::vector<Cell2D> cells;
  std::vector<Face2D> faces;
  std::vector<BoundaryPatch> patches;

  std::string token;
  while (input >> token) {
    if (token == "DIM") {
      input >> metadata.dimension;
    } else if (token == "NODES") {
      std::size_t count = 0;
      input >> count;
      nodes.resize(count);
      for (std::size_t i = 0; i < count; ++i) {
        IndexValue id = -1;
        input >> id >> nodes[i].point.x >> nodes[i].point.y;
        nodes[i].id = NodeIndex{id};
      }
    } else if (token == "CELLS") {
      std::size_t count = 0;
      input >> count;
      cells.resize(count);
      for (std::size_t i = 0; i < count; ++i) {
        IndexValue id = -1;
        input >> id >> cells[i].centre.x >> cells[i].centre.y >> cells[i].area;
        cells[i].id = CellIndex{id};
      }
    } else if (token == "FACES") {
      std::size_t count = 0;
      input >> count;
      faces.resize(count);
      for (std::size_t i = 0; i < count; ++i) {
        IndexValue id = -1;
        IndexValue owner = -1;
        IndexValue neighbour = -1;
        input >> id >> owner >> neighbour >> faces[i].centre.x >> faces[i].centre.y >>
            faces[i].unit_normal.x >> faces[i].unit_normal.y >> faces[i].length >> faces[i].patch_id;
        faces[i].id = FaceIndex{id};
        faces[i].owner = CellIndex{owner};
        faces[i].neighbour = CellIndex{neighbour};
      }
    } else if (token == "PATCHES") {
      std::size_t count = 0;
      input >> count;
      patches.resize(count);
      for (std::size_t i = 0; i < count; ++i) {
        input >> patches[i].id >> patches[i].name >> patches[i].type;
      }
    } else if (token == "END") {
      break;
    } else {
      Raise(Error("mesh.parse_error", "Unknown section in bootstrap mesh: " + token));
    }
  }

  MeshView mesh(std::move(nodes), std::move(cells), std::move(faces), std::move(patches),
                std::move(metadata));
  Require(mesh.IsValid(), "mesh.invalid_connectivity", "Bootstrap mesh failed basic validation.");
  return mesh;
}

}  // namespace mohidng
