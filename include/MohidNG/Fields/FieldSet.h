#pragma once

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <MohidNG/Core/ID.h>
#include <MohidNG/Fields/Field.h>
#include <MohidNG/Mesh/MeshView.h>


namespace mohidng {

class FieldSet {
 public:
  DefineIdentity("MohidNG.Fields.FieldSet")

  void AddScalar(ScalarField field);
  ScalarField& AddCellScalar(std::string name, const MeshView& mesh, Real initial_value = 0.0);
  ScalarField& AddFaceScalar(std::string name, const MeshView& mesh, Real initial_value = 0.0);
  ScalarField& AddNodeScalar(std::string name, const MeshView& mesh, Real initial_value = 0.0);

  [[nodiscard]] bool HasScalar(const std::string& name) const;
  [[nodiscard]] ScalarField& Scalar(const std::string& name);
  [[nodiscard]] const ScalarField& Scalar(const std::string& name) const;
  [[nodiscard]] std::vector<std::string> ScalarNames() const;

  void RequireScalar(const std::string& name, FieldLocation location, std::size_t size) const;

 private:
  [[nodiscard]] ScalarField& AddScalarAndReturn(ScalarField field);

  std::unordered_map<std::string, ScalarField> scalars_;
};

}  // namespace mohidng
