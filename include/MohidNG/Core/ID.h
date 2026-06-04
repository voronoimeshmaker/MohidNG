#pragma once

#include <string_view>


namespace mohidng {

struct ClassIdentity {
  std::string_view id;
  std::string_view name;
};

}  // namespace mohidng

#define DefineIdentity(identity_value) \
  static constexpr ::mohidng::ClassIdentity kIdentity{identity_value, identity_value}; \
  [[nodiscard]] static constexpr std::string_view ClassId() noexcept { return kIdentity.id; } \
  [[nodiscard]] static constexpr std::string_view ClassName() noexcept { return kIdentity.name; }

#define DefineNamedIdentity(identity_value, name_value) \
  static constexpr ::mohidng::ClassIdentity kIdentity{identity_value, name_value}; \
  [[nodiscard]] static constexpr std::string_view ClassId() noexcept { return kIdentity.id; } \
  [[nodiscard]] static constexpr std::string_view ClassName() noexcept { return kIdentity.name; }
