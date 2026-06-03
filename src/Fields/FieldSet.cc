#include "MohidNG/Fields/FieldSet.h"

#include "MohidNG/Core/Error.h"

namespace mohidng {

void FieldSet::AddScalar(ScalarField field) { scalars_.emplace(field.Name(), std::move(field)); }

bool FieldSet::HasScalar(const std::string& name) const { return scalars_.contains(name); }

ScalarField& FieldSet::Scalar(const std::string& name) {
  auto it = scalars_.find(name);
  Require(it != scalars_.end(), "Scalar field not found.");
  return it->second;
}

const ScalarField& FieldSet::Scalar(const std::string& name) const {
  auto it = scalars_.find(name);
  Require(it != scalars_.end(), "Scalar field not found.");
  return it->second;
}

}  // namespace mohidng
