#pragma once

#include <stdexcept>
#include <string>
#include <string_view>

namespace mohidng {

class MohidNgError : public std::runtime_error {
 public:
  explicit MohidNgError(std::string message);
};

void Require(bool condition, std::string_view message);

}  // namespace mohidng
