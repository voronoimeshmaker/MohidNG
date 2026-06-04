#pragma once

#include <span>
#include <string_view>
#include <vector>

#include <MohidNG/Core/ID.h>
#include <MohidNG/Mesh/MeshTypes.h>


namespace mohidng {

class MeshView {
 public:
  DefineIdentity("MohidNG.Mesh.MeshView")

  MeshView() = default;
  MeshView(std::vector<Node2D> nodes, std::vector<Cell2D> cells, std::vector<Face2D> faces,
           std::vector<BoundaryPatch> patches, MeshMetadata metadata);

  [[nodiscard]] std::span<const Node2D> Nodes() const;
  [[nodiscard]] std::span<const Cell2D> Cells() const;
  [[nodiscard]] std::span<const Face2D> Faces() const;
  [[nodiscard]] std::span<const BoundaryPatch> BoundaryPatches() const;
  [[nodiscard]] const MeshMetadata& Metadata() const;

  [[nodiscard]] const Cell2D& Cell(CellIndex id) const;
  [[nodiscard]] std::vector<CellIndex> CellNeighbours(CellIndex id) const;
  [[nodiscard]] bool IsBoundaryFace(const Face2D& face) const;
  [[nodiscard]] bool IsValid() const;

 private:
  std::vector<Node2D> nodes_;
  std::vector<Cell2D> cells_;
  std::vector<Face2D> faces_;
  std::vector<BoundaryPatch> patches_;
  MeshMetadata metadata_{};
};

}  // namespace mohidng
