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

  std::array<float, 4> cornerLights = GetCornerLightValues(x, y, z, face, chunk);
  switch (face) {
  case Direction::SOUTH: {
    return {
      x0, y1, z1, tex.x0, tex.y1, cornerLights[0],
      x0, y0, z1, tex.x0, tex.y0, cornerLights[1],
      x1, y0, z1, tex.x1, tex.y0, cornerLights[2],
      x1, y1, z1, tex.x1, tex.y1, cornerLights[3]
    };
  }
  case Direction::NORTH: {
    return {
      x1, y1, z0, tex.x0, tex.y1, cornerLights[0],
      x1, y0, z0, tex.x0, tex.y0, cornerLights[1],
      x0, y0, z0, tex.x1, tex.y0, cornerLights[2],
      x0, y1, z0, tex.x1, tex.y1, cornerLights[3]
    };
  }
  case Direction::EAST: {
    return {
      x1, y1, z1, tex.x0, tex.y1, cornerLights[0],
      x1, y0, z1, tex.x0, tex.y0, cornerLights[1],
      x1, y0, z0, tex.x1, tex.y0, cornerLights[2],
      x1, y1, z0, tex.x1, tex.y1, cornerLights[3]
    };
  }
  case Direction::WEST: {
    return {
      x0, y1, z0, tex.x0, tex.y1, cornerLights[0],
      x0, y0, z0, tex.x0, tex.y0, cornerLights[1],
      x0, y0, z1, tex.x1, tex.y0, cornerLights[2],
      x0, y1, z1, tex.x1, tex.y1, cornerLights[3]
    };
  }
  case Direction::UP: {
    return {
      x0, y1, z0, tex.x0, tex.y1, cornerLights[0],
      x0, y1, z1, tex.x0, tex.y0, cornerLights[1],
      x1, y1, z1, tex.x1, tex.y0, cornerLights[2],
      x1, y1, z0, tex.x1, tex.y1, cornerLights[3]
    };
  }
  case Direction::DOWN: {
    return {
      x1, y0, z0, tex.x0, tex.y1, cornerLights[0],
      x1, y0, z1, tex.x0, tex.y0, cornerLights[1],
      x0, y0, z1, tex.x1, tex.y0, cornerLights[2],
      x0, y0, z0, tex.x1, tex.y1, cornerLights[3]
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

std::array<float, 4> VoxelData::GetCornerLightValues(int x, int y, int z, Direction face, Chunk& chunk) {
  std::array<glm::ivec3, 9> offsets = GetOffset3x3(x, y, z, face);
  if (DebugSettings::instance.smoothLighting) {
    float l00 = chunk.GetFixedLightAt(XYZ(offsets[0]));
    float l10 = chunk.GetFixedLightAt(XYZ(offsets[1]));
    float l20 = chunk.GetFixedLightAt(XYZ(offsets[2]));
    float l01 = chunk.GetFixedLightAt(XYZ(offsets[3]));
    float l11 = chunk.GetFixedLightAt(XYZ(offsets[4]));
    float l21 = chunk.GetFixedLightAt(XYZ(offsets[5]));
    float l02 = chunk.GetFixedLightAt(XYZ(offsets[6]));
    float l12 = chunk.GetFixedLightAt(XYZ(offsets[7]));
    float l22 = chunk.GetFixedLightAt(XYZ(offsets[8]));

    bool s10 = Block::FromBlockstate(chunk.GetBlockstateAt(XYZ(offsets[1]))).IsSolid();
    bool s01 = Block::FromBlockstate(chunk.GetBlockstateAt(XYZ(offsets[3]))).IsSolid();
    bool s21 = Block::FromBlockstate(chunk.GetBlockstateAt(XYZ(offsets[5]))).IsSolid();
    bool s12 = Block::FromBlockstate(chunk.GetBlockstateAt(XYZ(offsets[7]))).IsSolid();

    // Don't consider corners if they're completely blocked
    if (s10 && s01) l00 = 0.0f;
    if (s10 && s21) l20 = 0.0f;
    if (s01 && s12) l02 = 0.0f;
    if (s12 && s21) l22 = 0.0f;

    return {
      NormalizeAmbientLighting(l00, l10, l01, l11),
      NormalizeAmbientLighting(l01, l11, l02, l12),
      NormalizeAmbientLighting(l11, l21, l12, l22),
      NormalizeAmbientLighting(l10, l20, l11, l21)
    };
  }

  float centerLight = chunk.GetFixedLightAt(XYZ(offsets[4]));

  return { centerLight, centerLight, centerLight, centerLight };
}

std::array<glm::ivec3, 9> VoxelData::GetOffset3x3(int x, int y, int z, Direction face) {
  switch (face) {
  case Direction::SOUTH:
    return {
      glm::ivec3{x - 1, y + 1, z + 1},
      glm::ivec3{x + 0, y + 1, z + 1},
      glm::ivec3{x + 1, y + 1, z + 1},
      glm::ivec3{x - 1, y + 0, z + 1},
      glm::ivec3{x + 0, y + 0, z + 1},
      glm::ivec3{x + 1, y + 0, z + 1},
      glm::ivec3{x - 1, y - 1, z + 1},
      glm::ivec3{x + 0, y - 1, z + 1},
      glm::ivec3{x + 1, y - 1, z + 1}
    };
  case Direction::NORTH:
    return {
      glm::ivec3{x + 1, y + 1, z - 1},
      glm::ivec3{x + 0, y + 1, z - 1},
      glm::ivec3{x - 1, y + 1, z - 1},
      glm::ivec3{x + 1, y + 0, z - 1},
      glm::ivec3{x + 0, y + 0, z - 1},
      glm::ivec3{x - 1, y + 0, z - 1},
      glm::ivec3{x + 1, y - 1, z - 1},
      glm::ivec3{x + 0, y - 1, z - 1},
      glm::ivec3{x - 1, y - 1, z - 1}
    };
  case Direction::EAST:
    return {
      glm::ivec3{x + 1, y + 1, z + 1},
      glm::ivec3{x + 1, y + 1, z + 0},
      glm::ivec3{x + 1, y + 1, z - 1},
      glm::ivec3{x + 1, y + 0, z + 1},
      glm::ivec3{x + 1, y + 0, z + 0},
      glm::ivec3{x + 1, y + 0, z - 1},
      glm::ivec3{x + 1, y - 1, z + 1},
      glm::ivec3{x + 1, y - 1, z + 0},
      glm::ivec3{x + 1, y - 1, z - 1}
    };
  case Direction::WEST:
    return {
      glm::ivec3{x - 1, y + 1, z - 1},
      glm::ivec3{x - 1, y + 1, z + 0},
      glm::ivec3{x - 1, y + 1, z + 1},
      glm::ivec3{x - 1, y + 0, z - 1},
      glm::ivec3{x - 1, y + 0, z + 0},
      glm::ivec3{x - 1, y + 0, z + 1},
      glm::ivec3{x - 1, y - 1, z - 1},
      glm::ivec3{x - 1, y - 1, z + 0},
      glm::ivec3{x - 1, y - 1, z + 1}
    };
  case Direction::UP:
    return {
      glm::ivec3 { x - 1, y + 1, z - 1 },
        glm::ivec3 { x + 0, y + 1, z - 1 },
        glm::ivec3 { x + 1, y + 1, z - 1 },
        glm::ivec3 { x - 1, y + 1, z + 0 },
        glm::ivec3 { x + 0, y + 1, z + 0 },
        glm::ivec3 { x + 1, y + 1, z + 0 },
        glm::ivec3 { x - 1, y + 1, z + 1 },
        glm::ivec3 { x + 0, y + 1, z + 1 },
        glm::ivec3 { x + 1, y + 1, z + 1 }
    };
  case Direction::DOWN:
    return {
      glm::ivec3{x + 1, y - 1, z - 1},
      glm::ivec3{x + 0, y - 1, z - 1},
      glm::ivec3{x - 1, y - 1, z - 1},
      glm::ivec3{x + 1, y - 1, z + 0},
      glm::ivec3{x + 0, y - 1, z + 0},
      glm::ivec3{x - 1, y - 1, z + 0},
      glm::ivec3{x + 1, y - 1, z + 1},
      glm::ivec3{x + 0, y - 1, z + 1},
      glm::ivec3{x - 1, y - 1, z + 1}
    };
  }
}
