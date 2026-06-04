#pragma once

#include <algorithm>
#include <cctype>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <MohidNG/Core/Error.h>
#include <MohidNG/Core/ID.h>
#include <MohidNG/Core/Index.h>
#include <MohidNG/Core/Types.h>
#include <MohidNG/Mesh/MeshTypes.h>


namespace mohidng {

class FieldLocation {
 public:
  DefineIdentity("MohidNG.Fields.FieldLocation")

  FieldLocation() = default;
  explicit FieldLocation(std::string name) : name_(Normalize(std::move(name))) {}

  [[nodiscard]] const std::string& Name() const noexcept { return name_; }

  friend bool operator==(const FieldLocation& lhs, const FieldLocation& rhs) noexcept {
    return lhs.name_ == rhs.name_;
  }

  friend bool operator!=(const FieldLocation& lhs, const FieldLocation& rhs) noexcept {
    return !(lhs == rhs);
  }

 private:
  [[nodiscard]] static std::string Normalize(std::string name) {
    RequireClass(FieldLocation, !name.empty(), "field.invalid_location", "Field location name cannot be empty.");
    std::ranges::transform(name, name.begin(), [](unsigned char c) {
      return static_cast<char>(std::tolower(c));
    });
    return name;
  }

  std::string name_ = "cell";
};

[[nodiscard]] inline FieldLocation CellFieldLocation() { return FieldLocation("cell"); }
[[nodiscard]] inline FieldLocation FaceFieldLocation() { return FieldLocation("face"); }
[[nodiscard]] inline FieldLocation NodeFieldLocation() { return FieldLocation("node"); }

template <typename T>
class Field {
 public:
  DefineIdentity("MohidNG.Fields.Field")

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
  FieldLocation location_ = CellFieldLocation();
  std::vector<T> values_;
};

using ScalarField = Field<Real>;
using Vector2Field = Field<Vec2>;

}  // namespace mohidng
