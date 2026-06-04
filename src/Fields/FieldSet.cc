#include "MohidNG/Fields/FieldSet.h"

#include "MohidNG/Core/Error.h"
#include "MohidNG/Core/Logger.h"

namespace mohidng {

void FieldSet::AddScalar(ScalarField field) {
  MOHIDNG_TRACE_SCOPE("FieldSet::AddScalar");
  scalars_.emplace(field.Name(), std::move(field));
}

bool FieldSet::HasScalar(const std::string& name) const { return scalars_.contains(name); }

ScalarField& FieldSet::Scalar(const std::string& name) {
  MOHIDNG_TRACE_SCOPE("FieldSet::Scalar");
  auto it = scalars_.find(name);
  Require(it != scalars_.end(), "field.not_found", name);
  return it->second;
}

const ScalarField& FieldSet::Scalar(const std::string& name) const {
  MOHIDNG_TRACE_SCOPE("FieldSet::Scalar const");
  auto it = scalars_.find(name);
  Require(it != scalars_.end(), "field.not_found", name);
  return it->second;
}

}  // namespace mohidng
