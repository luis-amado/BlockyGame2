#include "VoxelData.h"

#include "../world/Chunk.h"
#include "../init/Blocks.h"
#include "util/Logging.h"
#include "util/MathUtil.h"
#include "../debug/DebugSettings.h"
#include <array>

namespace {

float NormalizeAmbientLighting(float l1, float l2, float l3, float l4) {
  std::array<float*, 4> lightValues = { &l1, &l2, &l3, &l4 };
  float nonNegativeAverage = 0.0f;
  int nonNegativeCount = 0;

  for (float* value : lightValues) {
    if (*value > 0.0f) {
      nonNegativeCount++;
      nonNegativeAverage += *value;
    }
  }

  if (nonNegativeCount == 0) {
    nonNegativeAverage = 0.0f;
  } else {
    nonNegativeAverage /= (float)nonNegativeCount;
  }

  for (float* value : lightValues) {
    if (*value < 0.0f) {
      *value = std::max(floor(nonNegativeAverage) * 0.2f - 0.1f, -1.0f / 15.0f);
    }
  }

  return (l1 + l2 + l3 + l4) / 4.0f;
}

}  // namespace

std::vector<float> VoxelData::GetFaceVertices(int x, int y, int z, Chunk& chunk, Direction face, const Block& block) {
  int subchunkY = MathUtil::Mod(y, Chunk::SUBCHUNK_HEIGHT);
  float x0 = x, x1 = x + 1;
  float y0 = subchunkY, y1 = subchunkY + 1;
  float z0 = z, z1 = z + 1;

  const std::string& textureName = block.GetTextures()[face];
  TextureAtlas::TextureCoords tex = Blocks::GetAtlas().GetTextureCoords(textureName);

  switch (face) {
  case Direction::SOUTH: {
    float l00 = chunk.GetFixedLightAt(x - 1, y + 1, z + 1);
    float l10 = chunk.GetFixedLightAt(x + 0, y + 1, z + 1);
    float l20 = chunk.GetFixedLightAt(x + 1, y + 1, z + 1);
    float l01 = chunk.GetFixedLightAt(x - 1, y + 0, z + 1);
    float l11 = chunk.GetFixedLightAt(x + 0, y + 0, z + 1);
    float l21 = chunk.GetFixedLightAt(x + 1, y + 0, z + 1);
    float l02 = chunk.GetFixedLightAt(x - 1, y - 1, z + 1);
    float l12 = chunk.GetFixedLightAt(x + 0, y - 1, z + 1);
    float l22 = chunk.GetFixedLightAt(x + 1, y - 1, z + 1);
    return {
      x0, y1, z1, tex.x0, tex.y1, DebugSettings::instance.smoothLighting ? NormalizeAmbientLighting(l00, l10, l01, l11) : l11,
      x0, y0, z1, tex.x0, tex.y0, DebugSettings::instance.smoothLighting ? NormalizeAmbientLighting(l01, l11, l02, l12) : l11,
      x1, y0, z1, tex.x1, tex.y0, DebugSettings::instance.smoothLighting ? NormalizeAmbientLighting(l11, l21, l12, l22) : l11,
      x1, y1, z1, tex.x1, tex.y1, DebugSettings::instance.smoothLighting ? NormalizeAmbientLighting(l10, l20, l11, l21) : l11
    };
  }
  case Direction::NORTH: {
    float l00 = chunk.GetFixedLightAt(x + 1, y + 1, z - 1);
    float l10 = chunk.GetFixedLightAt(x + 0, y + 1, z - 1);
    float l20 = chunk.GetFixedLightAt(x - 1, y + 1, z - 1);
    float l01 = chunk.GetFixedLightAt(x + 1, y + 0, z - 1);
    float l11 = chunk.GetFixedLightAt(x + 0, y + 0, z - 1);
    float l21 = chunk.GetFixedLightAt(x - 1, y + 0, z - 1);
    float l02 = chunk.GetFixedLightAt(x + 1, y - 1, z - 1);
    float l12 = chunk.GetFixedLightAt(x + 0, y - 1, z - 1);
    float l22 = chunk.GetFixedLightAt(x - 1, y - 1, z - 1);
    return {
      x1, y1, z0, tex.x0, tex.y1, DebugSettings::instance.smoothLighting ? NormalizeAmbientLighting(l00, l10, l01, l11) : l11,
      x1, y0, z0, tex.x0, tex.y0, DebugSettings::instance.smoothLighting ? NormalizeAmbientLighting(l01, l11, l02, l12) : l11,
      x0, y0, z0, tex.x1, tex.y0, DebugSettings::instance.smoothLighting ? NormalizeAmbientLighting(l11, l21, l12, l22) : l11,
      x0, y1, z0, tex.x1, tex.y1, DebugSettings::instance.smoothLighting ? NormalizeAmbientLighting(l10, l20, l11, l21) : l11
    };
  }
  case Direction::EAST: {
    float l00 = chunk.GetFixedLightAt(x + 1, y + 1, z + 1);
    float l10 = chunk.GetFixedLightAt(x + 1, y + 1, z + 0);
    float l20 = chunk.GetFixedLightAt(x + 1, y + 1, z - 1);
    float l01 = chunk.GetFixedLightAt(x + 1, y + 0, z + 1);
    float l11 = chunk.GetFixedLightAt(x + 1, y + 0, z + 0);
    float l21 = chunk.GetFixedLightAt(x + 1, y + 0, z - 1);
    float l02 = chunk.GetFixedLightAt(x + 1, y - 1, z + 1);
    float l12 = chunk.GetFixedLightAt(x + 1, y - 1, z + 0);
    float l22 = chunk.GetFixedLightAt(x + 1, y - 1, z - 1);
    return {
      x1, y1, z1, tex.x0, tex.y1, DebugSettings::instance.smoothLighting ? NormalizeAmbientLighting(l00, l10, l01, l11) : l11,
      x1, y0, z1, tex.x0, tex.y0, DebugSettings::instance.smoothLighting ? NormalizeAmbientLighting(l01, l11, l02, l12) : l11,
      x1, y0, z0, tex.x1, tex.y0, DebugSettings::instance.smoothLighting ? NormalizeAmbientLighting(l11, l21, l12, l22) : l11,
      x1, y1, z0, tex.x1, tex.y1, DebugSettings::instance.smoothLighting ? NormalizeAmbientLighting(l10, l20, l11, l21) : l11
    };
  }
  case Direction::WEST: {
    float l00 = chunk.GetFixedLightAt(x - 1, y + 1, z - 1);
    float l10 = chunk.GetFixedLightAt(x - 1, y + 1, z + 0);
    float l20 = chunk.GetFixedLightAt(x - 1, y + 1, z + 1);
    float l01 = chunk.GetFixedLightAt(x - 1, y + 0, z - 1);
    float l11 = chunk.GetFixedLightAt(x - 1, y + 0, z + 0);
    float l21 = chunk.GetFixedLightAt(x - 1, y + 0, z + 1);
    float l02 = chunk.GetFixedLightAt(x - 1, y - 1, z - 1);
    float l12 = chunk.GetFixedLightAt(x - 1, y - 1, z + 0);
    float l22 = chunk.GetFixedLightAt(x - 1, y - 1, z + 1);
    return {
      x0, y1, z0, tex.x0, tex.y1, DebugSettings::instance.smoothLighting ? NormalizeAmbientLighting(l00, l10, l01, l11) : l11,
      x0, y0, z0, tex.x0, tex.y0, DebugSettings::instance.smoothLighting ? NormalizeAmbientLighting(l01, l11, l02, l12) : l11,
      x0, y0, z1, tex.x1, tex.y0, DebugSettings::instance.smoothLighting ? NormalizeAmbientLighting(l11, l21, l12, l22) : l11,
      x0, y1, z1, tex.x1, tex.y1, DebugSettings::instance.smoothLighting ? NormalizeAmbientLighting(l10, l20, l11, l21) : l11
    };
  }
  case Direction::UP: {
    float l00 = chunk.GetFixedLightAt(x - 1, y + 1, z - 1);
    float l10 = chunk.GetFixedLightAt(x + 0, y + 1, z - 1);
    float l20 = chunk.GetFixedLightAt(x + 1, y + 1, z - 1);
    float l01 = chunk.GetFixedLightAt(x - 1, y + 1, z + 0);
    float l11 = chunk.GetFixedLightAt(x + 0, y + 1, z + 0);
    float l21 = chunk.GetFixedLightAt(x + 1, y + 1, z + 0);
    float l02 = chunk.GetFixedLightAt(x - 1, y + 1, z + 1);
    float l12 = chunk.GetFixedLightAt(x + 0, y + 1, z + 1);
    float l22 = chunk.GetFixedLightAt(x + 1, y + 1, z + 1);
    return {
      x0, y1, z0, tex.x0, tex.y1, DebugSettings::instance.smoothLighting ? NormalizeAmbientLighting(l00, l10, l01, l11) : l11,
      x0, y1, z1, tex.x0, tex.y0, DebugSettings::instance.smoothLighting ? NormalizeAmbientLighting(l01, l11, l02, l12) : l11,
      x1, y1, z1, tex.x1, tex.y0, DebugSettings::instance.smoothLighting ? NormalizeAmbientLighting(l11, l21, l12, l22) : l11,
      x1, y1, z0, tex.x1, tex.y1, DebugSettings::instance.smoothLighting ? NormalizeAmbientLighting(l10, l20, l11, l21) : l11
    };
  }
  case Direction::DOWN: {
    float l00 = chunk.GetFixedLightAt(x + 1, y - 1, z - 1);
    float l10 = chunk.GetFixedLightAt(x + 0, y - 1, z - 1);
    float l20 = chunk.GetFixedLightAt(x - 1, y - 1, z - 1);
    float l01 = chunk.GetFixedLightAt(x + 1, y - 1, z + 0);
    float l11 = chunk.GetFixedLightAt(x + 0, y - 1, z + 0);
    float l21 = chunk.GetFixedLightAt(x - 1, y - 1, z + 0);
    float l02 = chunk.GetFixedLightAt(x + 1, y - 1, z + 1);
    float l12 = chunk.GetFixedLightAt(x + 0, y - 1, z + 1);
    float l22 = chunk.GetFixedLightAt(x - 1, y - 1, z + 1);
    return {
      x1, y0, z0, tex.x0, tex.y1, DebugSettings::instance.smoothLighting ? NormalizeAmbientLighting(l00, l10, l01, l11) : l11,
      x1, y0, z1, tex.x0, tex.y0, DebugSettings::instance.smoothLighting ? NormalizeAmbientLighting(l01, l11, l02, l12) : l11,
      x0, y0, z1, tex.x1, tex.y0, DebugSettings::instance.smoothLighting ? NormalizeAmbientLighting(l11, l21, l12, l22) : l11,
      x0, y0, z0, tex.x1, tex.y1, DebugSettings::instance.smoothLighting ? NormalizeAmbientLighting(l10, l20, l11, l21) : l11
    };
  }
  }
}

glm::ivec3 VoxelData::GetFaceOffset(Direction face) {
  switch (face) {
  case Direction::SOUTH:
    return { 0, 0, 1 };
  case Direction::NORTH:
    return { 0, 0, -1 };
  case Direction::EAST:
    return { 1, 0, 0 };
  case Direction::WEST:
    return { -1, 0, 0 };
  case Direction::UP:
    return { 0, 1, 0 };
  case Direction::DOWN:
    return { 0, -1, 0 };
  };
}

const std::vector<glm::ivec3>& VoxelData::GetNeighborOffsetsAndOrigin() {
  static std::vector<glm::ivec3> neighborOffsets = {
    { 0, 0, 0},
    { 1, 0, 0},
    {-1, 0, 0},
    { 0, 1, 0},
    { 0,-1, 0},
    { 0, 0, 1},
    { 0, 0,-1},
  };
  return neighborOffsets;
}
