#pragma once

#include <string>
#include <array>
#include "../voxel/Direction.h"

struct BlockTextures {
  std::string north, south, east, west, up, down;

  static BlockTextures All(const std::string& all) {
    return { all, all, all, all, all, all };
  }

  static BlockTextures SideUpDown(const std::string& side, const std::string& up, const std::string& down) {
    return { side, side, side, side, up, down };
  }

  static BlockTextures SideEnd(const std::string& side, const std::string& end) {
    return { side, side, side, side, end, end };
  }

  BlockTextures() = default;
  BlockTextures(const std::string& _north, const std::string& _south, const std::string& _east, const std::string& _west, const std::string& _up, const std::string& _down)
    : north(_north), south(_south), east(_east), west(_west), up(_up), down(_down) {}

  const std::string& operator[](Direction face) const {
    switch (face) {
    case Direction::NORTH: return north;
    case Direction::SOUTH: return south;
    case Direction::EAST: return east;
    case Direction::WEST: return west;
    case Direction::UP: return up;
    case Direction::DOWN: return down;
    }
  }

  std::array<std::string, 6> GetAll() const {
    return std::array<std::string, 6>{ north, south, east, west, up, down };
  }
};