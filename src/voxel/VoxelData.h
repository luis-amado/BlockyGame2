#pragma once

#include <vector>
#include <glm/vec3.hpp>

#include "Direction.h"
#include "../block/Block.h"
#include "rendering/textures/TextureAtlas.h"

class Chunk;

class VoxelData {
public:
  static std::vector<float> GetFaceVertices(int x, int y, int z, Chunk& chunk, Direction face, const Block& block);
  static glm::ivec3 GetFaceOffset(Direction face);

  static const std::vector<glm::ivec3>& GetNeighborOffsetsAndOrigin();
};