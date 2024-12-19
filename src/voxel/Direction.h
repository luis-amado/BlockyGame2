#pragma once

#include <vector>

enum class Direction {
  SOUTH,  // +z
  NORTH,  // -z
  EAST,   // +x
  WEST,   // -x
  UP,     // +y
  DOWN    // -y
};

class DirectionUtil {
public:
  static const std::vector<Direction>& GetAllDirections();
private:
  static const std::vector<Direction> s_allDirections;
};