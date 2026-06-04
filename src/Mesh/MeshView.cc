#include <algorithm>

#include <MohidNG/Core/Error.h>
#include <MohidNG/Core/Logger.h>
#include <MohidNG/Mesh/MeshView.h>


namespace mohidng {

MeshView::MeshView(std::vector<Node2D> nodes, std::vector<Cell2D> cells,
                   std::vector<Face2D> faces, std::vector<BoundaryPatch> patches,
                   MeshMetadata metadata)
    : nodes_(std::move(nodes)),
      cells_(std::move(cells)),
      faces_(std::move(faces)),
      patches_(std::move(patches)),
      metadata_(std::move(metadata)) {}

std::span<const Node2D> MeshView::Nodes() const { return nodes_; }

std::span<const Cell2D> MeshView::Cells() const { return cells_; }

std::span<const Face2D> MeshView::Faces() const { return faces_; }

std::span<const BoundaryPatch> MeshView::BoundaryPatches() const { return patches_; }

const MeshMetadata& MeshView::Metadata() const { return metadata_; }

const Cell2D& MeshView::Cell(CellIndex id) const {
  MOHIDNG_TRACE_CLASS(MeshView::ClassId(), "Cell");
  RequireClass(MeshView, mohidng::IsValid(id), "mesh.invalid_connectivity", "Invalid cell index.");
  RequireClass(MeshView, static_cast<std::size_t>(id.value) < cells_.size(),
               "mesh.invalid_connectivity", "Cell index out of range.");
  return cells_.at(static_cast<std::size_t>(id.value));
}

std::vector<CellIndex> MeshView::CellNeighbours(CellIndex id) const {
  MOHIDNG_TRACE_CLASS(MeshView::ClassId(), "CellNeighbours");
  std::vector<CellIndex> neighbours;
  for (const auto& face : faces_) {
    if (face.owner.value == id.value && mohidng::IsValid(face.neighbour)) {
      neighbours.push_back(face.neighbour);
    } else if (face.neighbour.value == id.value && mohidng::IsValid(face.owner)) {
      neighbours.push_back(face.owner);
    }
  }
  return neighbours;
}

bool MeshView::IsBoundaryFace(const Face2D& face) const { return !mohidng::IsValid(face.neighbour); }

bool MeshView::IsValid() const {
  MOHIDNG_TRACE_CLASS(MeshView::ClassId(), "IsValid");
  if (metadata_.dimension != 2) {
    return false;
  }
  for (const auto& cell : cells_) {
    if (!mohidng::IsValid(cell.id) || cell.area <= 0.0) {
      return false;
    }
  }
  for (const auto& face : faces_) {
    if (!mohidng::IsValid(face.id) || !mohidng::IsValid(face.owner) || face.length <= 0.0) {
      return false;
    }
  }
  return true;
}

}  // namespace mohidng
