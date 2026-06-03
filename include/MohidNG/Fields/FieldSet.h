#pragma once

#include <string>
#include <unordered_map>

#include "MohidNG/Fields/Field.h"

namespace mohidng {

class FieldSet {
 public:
  void AddScalar(ScalarField field);
  [[nodiscard]] bool HasScalar(const std::string& name) const;
  [[nodiscard]] ScalarField& Scalar(const std::string& name);
  [[nodiscard]] const ScalarField& Scalar(const std::string& name) const;

 private:
  std::unordered_map<std::string, ScalarField> scalars_;
};

}  // namespace mohidng
