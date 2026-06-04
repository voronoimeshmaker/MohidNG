#pragma once

#include <MohidNG/Core/Types.h>


namespace mohidng {

using IndexValue = Integer;

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
