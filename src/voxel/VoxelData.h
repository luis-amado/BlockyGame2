#pragma once

#include <vector>
#include <glm/vec3.hpp>

#include "Direction.h"
#include "../block/Block.h"
#include "rendering/textures/TextureAtlas.h"

class VoxelData {
public:
  static std::vector<float> GetFaceVertices(int x, int y, int z, Direction face, const Block& block);
  static glm::ivec3 GetFaceOffset(Direction face);
};