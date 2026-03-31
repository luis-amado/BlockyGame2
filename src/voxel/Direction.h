#pragma once

#include <vector>
#include <iostream>

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
  static std::string ToString(Direction& dir);
  static std::string GetOffsetAsString(Direction& dir);
  static const std::vector<Direction>& GetAllDirections();
  static const Direction GetHorizontalFacingAxis(double rotY);
  static const Direction GetFacingAxis(double rotX, double rotY);
private:
  static const std::vector<Direction> s_allDirections;
};