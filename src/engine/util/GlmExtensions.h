#pragma once

#include <glm/glm.hpp>
#include <glm/vec2.hpp>
#include <functional>

struct IVec2Hash {
  std::size_t operator()(const glm::ivec2& vec) const noexcept {
    return std::hash<int>()(vec.x) ^ (std::hash<int>()(vec.y) << 1);
  }
};

struct IVec2Equal {
  bool operator()(const glm::ivec2& a, const glm::ivec2& b) const noexcept {
    return a.x == b.x && a.y == b.y;
  }
};

struct IVec3Hash {
  std::size_t operator()(const glm::ivec3& vec) const noexcept {
    return std::hash<int>()(vec.x) ^ (std::hash<int>()(vec.y) << 2) ^ (std::hash<int>()(vec.z) >> 2);
  }
};

struct IVec3Equal {
  bool operator()(const glm::ivec3& a, const glm::ivec3& b) const noexcept {
    return a.x == b.x && a.y == b.y && a.z == b.z;
  }
};