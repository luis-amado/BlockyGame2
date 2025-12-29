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

private:
  static std::array<float, 4> GetCornerLightValues(int x, int y, int z, Direction face, Chunk& chunk);
  static std::array<glm::ivec3, 9> GetOffset3x3(int x, int y, int z, Direction face);
};