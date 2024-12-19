#include "Direction.h"

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