#include "AABB.h"

#include <glm/vec3.hpp>
#include <cmath>
#include <algorithm>
#include <limits>
#include <numbers>

#include "util/MathUtil.h"

AABB::AABB(glm::dvec3 position, double width, double height)
  : m_position(position), m_width(width), m_height(height) {}

AABB AABB::CreateFromMinCorner(glm::dvec3 position, double width, double height) {
  return AABB(position, width, height);
}

AABB AABB::CreateFromBottomCenter(glm::dvec3 position, double width, double height) {
  glm::dvec3 cornerPosition = position - glm::dvec3 { width / 2.0, 0.0, width / 2.0 };
  return AABB(cornerPosition, width, height);
}

AABB AABB::CreateFromCenter(glm::dvec3 position, double width, double height) {
  glm::dvec3 cornerPosition = position - glm::dvec3 { width / 2.0, height / 2.0, width / 2.0 };
  return AABB(cornerPosition, width, height);
}

AxisRanges AABB::CalculateBlocksInsideBox(const glm::dvec3& delta) {
  AxisRanges ranges;

  glm::dvec3 newPosition = m_position + delta;

  double minX = std::min(newPosition.x, m_position.x);
  double minY = std::min(newPosition.y, m_position.y);
  double minZ = std::min(newPosition.z, m_position.z);

  double maxX = std::max(newPosition.x, m_position.x);
  double maxY = std::max(newPosition.y, m_position.y);
  double maxZ = std::max(newPosition.z, m_position.z);

  ranges.minX = std::floor(minX);
  ranges.maxX = std::floor(maxX + m_width);
  ranges.minY = std::floor(minY);
  ranges.maxY = std::floor(maxY + m_height);
  ranges.minZ = std::floor(minZ);
  ranges.maxZ = std::floor(maxZ + m_width);

  return ranges;
}

SweptCollisionResult AABB::SweptCollisionDetection(const glm::dvec3& velocity, const AABB& box2) {
  float xInvEntry, yInvEntry, zInvEntry;
  float xInvExit, yInvExit, zInvExit;

  // find the distance between the objects on the near and far sides for x, y, z
  if (velocity.x > 0.0) {
    xInvEntry = box2.m_position.x - (m_position.x + m_width);
    xInvExit = (box2.m_position.x + box2.m_width) - m_position.x;
  } else {
    xInvEntry = (box2.m_position.x + box2.m_width) - m_position.x;
    xInvExit = box2.m_position.x - (m_position.x + m_width);
  }

  if (velocity.y > 0.0) {
    yInvEntry = box2.m_position.y - (m_position.y + m_height);
    yInvExit = (box2.m_position.y + box2.m_height) - m_position.y;
  } else {
    yInvEntry = (box2.m_position.y + box2.m_height) - m_position.y;
    yInvExit = box2.m_position.y - (m_position.y + m_height);
  }

  if (velocity.z > 0.0) {
    zInvEntry = box2.m_position.z - (m_position.z + m_width);
    zInvExit = (box2.m_position.z + box2.m_width) - m_position.z;
  } else {
    zInvEntry = (box2.m_position.z + box2.m_width) - m_position.z;
    zInvExit = box2.m_position.z - (m_position.z + m_width);
  }

  // find the time of collision and time of leaving for each axis
  float xEntry, yEntry, zEntry;
  float xExit, yExit, zExit;

  if (velocity.x == 0.0) {
    xEntry = -std::numeric_limits<float>::infinity();
    xExit = std::numeric_limits<float>::infinity();
  } else {
    xEntry = xInvEntry / velocity.x;
    xExit = xInvExit / velocity.x;
  }

  if (velocity.y == 0.0) {
    yEntry = -std::numeric_limits<float>::infinity();
    yExit = std::numeric_limits<float>::infinity();
  } else {
    yEntry = yInvEntry / velocity.y;
    yExit = yInvExit / velocity.y;
  }

  if (velocity.z == 0.0) {
    zEntry = -std::numeric_limits<float>::infinity();
    zExit = std::numeric_limits<float>::infinity();
  } else {
    zEntry = zInvEntry / velocity.z;
    zExit = zInvExit / velocity.z;
  }

  float entryTime = std::max(std::max(xEntry, yEntry), zEntry);
  float exitTime = std::min(std::min(xExit, yExit), zExit);

  // if there was no collision
  if (entryTime > exitTime || (xEntry < 0.0f && yEntry < 0.0f && zEntry < 0.0f) || xEntry > 1.0f || yEntry > 1.0f || zEntry > 1.0f) {
    return {
      .time = 1.0f,
      .normal = {0.0, 0.0, 0.0},
    };
  }

  // calculate normal of collided surface
  glm::dvec3 normal = { 0.0, 0.0, 0.0 };
  if (yEntry >= xEntry && yEntry >= zEntry) {
    normal.y = -MathUtil::Sign(yInvEntry);
  } else if (xEntry >= yEntry && xEntry >= zEntry) {
    normal.x = -MathUtil::Sign(xInvEntry);
  } else {
    normal.z = -MathUtil::Sign(zInvEntry);
  }

  return {
    .time = entryTime,
    .normal = normal,
  };

}
