#pragma once

#include <vector>
#include <glm/vec3.hpp>

#include "Direction.h"

class VoxelData {
public:
  static std::vector<float> GetFaceVertices(int x, int y, int z, Direction face);
  static glm::ivec3 GetFaceOffset(Direction face);
};