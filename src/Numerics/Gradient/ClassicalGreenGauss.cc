#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <MohidNG/Core/ID.h>
#include <MohidNG/Mesh/MeshView.h>
#include <MohidNG/Numerics/Gradient/Gradient.h>


namespace mohidng {
namespace {

[[nodiscard]] bool SameCell(CellIndex left, CellIndex right) { return left.value == right.value; }

class ClassicalGreenGaussGradientMethod {
 public:
  DefineIdentity("MohidNG.Numerics.Gradient.ClassicalGreenGaussGradientMethod")

  [[nodiscard]] static constexpr std::string_view Name() { return "classical_green_gauss"; }

  [[nodiscard]] static CellGradientWorkspace BuildWorkspace(const MeshView& mesh,
                                                            const GradientOptions&) {
    std::vector<CellGradientStencil> stencils;
    stencils.reserve(mesh.Cells().size());
    for (const auto& cell : mesh.Cells()) {
      stencils.push_back(BuildStencilForCell(mesh, cell));
    }
    return CellGradientWorkspace(std::move(stencils), std::string(Name()), "not_used");
  }

 private:
  [[nodiscard]] static Vec2 OutwardNormalLengthForCell(const Face2D& face, const Cell2D& cell) {
    const Real sign = SameCell(face.owner, cell.id) ? 1.0 : -1.0;
    return Vec2{sign * face.unit_normal.x * face.length, sign * face.unit_normal.y * face.length};
  }

  [[nodiscard]] static CellGradientStencil BuildStencilForCell(const MeshView& mesh,
                                                               const Cell2D& cell) {
    CellGradientStencil stencil;
    stencil.cell = cell.id;
    stencil.determinant = cell.area;
    if (cell.area <= 0.0) {
      return stencil;
    }

    for (const auto& face : mesh.Faces()) {
      CellIndex neighbour{};
      if (SameCell(face.owner, cell.id)) {
        neighbour = face.neighbour;
      } else if (SameCell(face.neighbour, cell.id)) {
        neighbour = face.owner;
      } else {
        continue;
      }

      if (!mohidng::IsValid(neighbour)) {
        continue;
      }

      const auto normal_length = OutwardNormalLengthForCell(face, cell);
      stencil.coefficients.push_back(CellGradientCoefficient{
          neighbour,
          Vec2{0.5 * normal_length.x / cell.area, 0.5 * normal_length.y / cell.area}});
    }

    stencil.well_conditioned = true;
    return stencil;
  }
};

}  // namespace

void RegisterClassicalGreenGaussGradientMethod() {
  RegisterCellGradientMethod<ClassicalGreenGaussGradientMethod>();
}

}  // namespace mohidng
