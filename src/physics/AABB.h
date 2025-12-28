#pragma once

#include <glm/vec3.hpp>

struct AxisRanges {
  int minX, maxX, minY, maxY, minZ, maxZ;
};

struct SweptCollisionResult {
  float time;
  glm::dvec3 normal;
};

class AABB {
public:
  static AABB CreateFromMinCorner(glm::dvec3 position, double width, double height);
  static AABB CreateFromBottomCenter(glm::dvec3 position, double width, double height);
  static AABB CreateFromCenter(glm::dvec3 position, double width, double height);

  AxisRanges CalculateBlocksInsideBox(const glm::dvec3& velocity);

  SweptCollisionResult SweptCollisionDetection(const glm::dvec3& velocity, const AABB& box2);
  bool IsColliding(const AABB& other) const;

  glm::dvec3 GetMin() const;
  glm::dvec3 GetMax() const;

private:
  glm::dvec3 m_position;
  double m_width, m_height;

  AABB(glm::dvec3 m_position, double m_width, double m_height);
};