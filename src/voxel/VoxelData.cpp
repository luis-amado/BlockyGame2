#include "VoxelData.h"

#include "../world/Chunk.h"
#include "../init/Blocks.h"

std::vector<float> VoxelData::GetFaceVertices(int x, int y, int z, Chunk& chunk, Direction face, const Block& block) {
  float x0 = x, x1 = x + 1;
  float y0 = y, y1 = y + 1;
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
      x0, y1, z1, tex.x0, tex.y1, (l00 + l10 + l01 + l11) / 4.0f,
      x0, y0, z1, tex.x0, tex.y0, (l01 + l11 + l02 + l12) / 4.0f,
      x1, y0, z1, tex.x1, tex.y0, (l11 + l21 + l12 + l22) / 4.0f,
      x1, y1, z1, tex.x1, tex.y1, (l10 + l20 + l11 + l21) / 4.0f
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
      x1, y1, z0, tex.x0, tex.y1, (l00 + l10 + l01 + l11) / 4.0f,
      x1, y0, z0, tex.x0, tex.y0, (l01 + l11 + l02 + l12) / 4.0f,
      x0, y0, z0, tex.x1, tex.y0, (l11 + l21 + l12 + l22) / 4.0f,
      x0, y1, z0, tex.x1, tex.y1, (l10 + l20 + l11 + l21) / 4.0f
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
      x1, y1, z1, tex.x0, tex.y1, (l00 + l10 + l01 + l11) / 4.0f,
      x1, y0, z1, tex.x0, tex.y0, (l01 + l11 + l02 + l12) / 4.0f,
      x1, y0, z0, tex.x1, tex.y0, (l11 + l21 + l12 + l22) / 4.0f,
      x1, y1, z0, tex.x1, tex.y1, (l10 + l20 + l11 + l21) / 4.0f
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
      x0, y1, z0, tex.x0, tex.y1, (l00 + l10 + l01 + l11) / 4.0f,
      x0, y0, z0, tex.x0, tex.y0, (l01 + l11 + l02 + l12) / 4.0f,
      x0, y0, z1, tex.x1, tex.y0, (l11 + l21 + l12 + l22) / 4.0f,
      x0, y1, z1, tex.x1, tex.y1, (l10 + l20 + l11 + l21) / 4.0f
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
      x0, y1, z0, tex.x0, tex.y1, (l00 + l10 + l01 + l11) / 4.0f,
      x0, y1, z1, tex.x0, tex.y0, (l01 + l11 + l02 + l12) / 4.0f,
      x1, y1, z1, tex.x1, tex.y0, (l11 + l21 + l12 + l22) / 4.0f,
      x1, y1, z0, tex.x1, tex.y1, (l10 + l20 + l11 + l21) / 4.0f
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
      x1, y0, z0, tex.x0, tex.y1, (l00 + l10 + l01 + l11) / 4.0f,
      x1, y0, z1, tex.x0, tex.y0, (l01 + l11 + l02 + l12) / 4.0f,
      x0, y0, z1, tex.x1, tex.y0, (l11 + l21 + l12 + l22) / 4.0f,
      x0, y0, z0, tex.x1, tex.y1, (l10 + l20 + l11 + l21) / 4.0f
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
