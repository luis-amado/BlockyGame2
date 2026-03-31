#include "Direction.h"

std::string DirectionUtil::ToString(Direction& dir) {
  switch (dir) {
  case Direction::SOUTH: return "South";
  case Direction::NORTH: return "North";
  case Direction::EAST:  return "East";
  case Direction::WEST:  return "West";
  case Direction::UP:    return "Up";
  case Direction::DOWN:  return "Down";
  }
}

std::string DirectionUtil::GetOffsetAsString(Direction& dir) {
  switch (dir) {
  case Direction::SOUTH: return "+ z";
  case Direction::NORTH: return "- z";
  case Direction::EAST:  return "+ x";
  case Direction::WEST:  return "- x";
  case Direction::UP:    return "+ y";
  case Direction::DOWN:  return "- y";
  }
}

const std::vector<Direction> DirectionUtil::s_allDirections = {
  Direction::SOUTH,
  Direction::NORTH,
  Direction::EAST,
  Direction::WEST,
  Direction::UP,
  Direction::DOWN
};

const std::vector<Direction>& DirectionUtil::GetAllDirections() {
  return s_allDirections;
}

const Direction DirectionUtil::GetHorizontalFacingAxis(double rotY) {
  // rotY is an angle between 0 and 360
  if (rotY <= 45 || rotY > 325) {
    return Direction::NORTH;
  } else if (rotY <= 135) {
    return Direction::EAST;
  } else if (rotY <= 225) {
    return Direction::SOUTH;
  } else {
    return Direction::WEST;
  }

}

const Direction DirectionUtil::GetFacingAxis(double rotX, double rotY) {
  // TODO: not implemented
  return Direction::NORTH;
}
