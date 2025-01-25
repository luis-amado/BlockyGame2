#include "VoxelData.h"

#include "../init/Blocks.h"

std::vector<float> VoxelData::GetFaceVertices(int x, int y, int z, Direction face, const Block& block) {
  float x0 = x, x1 = x + 1;
  float y0 = y, y1 = y + 1;
  float z0 = z, z1 = z + 1;

  const std::string& textureName = block.GetTextures()[face];
  TextureAtlas::TextureCoords tex = Blocks::GetAtlas().GetTextureCoords(textureName);

  // TextureAtlas::TextureCoords tex = { 0.0f, 0.0f, 1.0f, 1.0f };

  switch (face) {
  case Direction::SOUTH:
    return {
      x0, y1, z1, tex.x0, tex.y1,
      x0, y0, z1, tex.x0, tex.y0,
      x1, y0, z1, tex.x1, tex.y0,
      x1, y1, z1, tex.x1, tex.y1
    };
  case Direction::NORTH:
    return {
      x1, y1, z0, tex.x0, tex.y1,
      x1, y0, z0, tex.x0, tex.y0,
      x0, y0, z0, tex.x1, tex.y0,
      x0, y1, z0, tex.x1, tex.y1
    };
  case Direction::EAST:
    return {
      x1, y1, z1, tex.x0, tex.y1,
      x1, y0, z1, tex.x0, tex.y0,
      x1, y0, z0, tex.x1, tex.y0,
      x1, y1, z0, tex.x1, tex.y1
    };
  case Direction::WEST:
    return {
      x0, y1, z0, tex.x0, tex.y1,
      x0, y0, z0, tex.x0, tex.y0,
      x0, y0, z1, tex.x1, tex.y0,
      x0, y1, z1, tex.x1, tex.y1
    };
  case Direction::UP:
    return {
      x0, y1, z0, tex.x0, tex.y1,
      x0, y1, z1, tex.x0, tex.y0,
      x1, y1, z1, tex.x1, tex.y0,
      x1, y1, z0, tex.x1, tex.y1
    };
  case Direction::DOWN:
    return {
      x1, y0, z0, tex.x0, tex.y1,
      x1, y0, z1, tex.x0, tex.y0,
      x0, y0, z1, tex.x1, tex.y0,
      x0, y0, z0, tex.x1, tex.y1
    };
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
