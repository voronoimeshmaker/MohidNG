#include "MohidNG/Core/Error.h"

namespace mohidng {

MohidNgError::MohidNgError(std::string message) : std::runtime_error(std::move(message)) {}

void Require(bool condition, std::string_view message) {
  if (!condition) {
    throw MohidNgError(std::string(message));
  }
}

}  // namespace mohidng
