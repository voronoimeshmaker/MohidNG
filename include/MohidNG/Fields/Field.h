#pragma once

#include <span>
#include <string>
#include <vector>

#include "MohidNG/Core/Error.h"
#include "MohidNG/Core/Index.h"
#include "MohidNG/Mesh/MeshTypes.h"

namespace mohidng {

enum class FieldLocation { kCell, kFace, kNode, kLayer, kParticle };

template <typename T>
class Field {
 public:
  Field() = default;
  Field(std::string name, FieldLocation location, std::size_t size)
      : name_(std::move(name)), location_(location), values_(size) {}

  [[nodiscard]] const std::string& Name() const { return name_; }
  [[nodiscard]] FieldLocation Location() const { return location_; }
  [[nodiscard]] std::size_t Size() const { return values_.size(); }

  [[nodiscard]] std::span<T> Values() { return values_; }
  [[nodiscard]] std::span<const T> Values() const { return values_; }

  [[nodiscard]] T& operator[](std::size_t index) { return values_.at(index); }
  [[nodiscard]] const T& operator[](std::size_t index) const { return values_.at(index); }

 private:
  std::string name_;
  FieldLocation location_ = FieldLocation::kCell;
  std::vector<T> values_;
};

using ScalarField = Field<double>;
using Vector2Field = Field<Vec2>;

}  // namespace mohidng
