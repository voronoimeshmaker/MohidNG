#pragma once

#include <cstddef>
#include <cstdint>

namespace mohidng {

using IndexValue = std::int64_t;

struct CellIndex {
  IndexValue value = -1;
};

struct FaceIndex {
  IndexValue value = -1;
};

struct NodeIndex {
  IndexValue value = -1;
};

constexpr bool IsValid(CellIndex index) { return index.value >= 0; }
constexpr bool IsValid(FaceIndex index) { return index.value >= 0; }
constexpr bool IsValid(NodeIndex index) { return index.value >= 0; }

}  // namespace mohidng
