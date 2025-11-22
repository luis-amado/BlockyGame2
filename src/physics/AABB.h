#pragma once

#include <glm/vec3.hpp>

class AABB {
public:
  static AABB CreateFromMinCorner(glm::dvec3 position, double width, double height);
  static AABB CreateFromBottomCenter(glm::dvec3 position, double width, double height);
  static AABB CreateFromCenter(glm::dvec3 position, double width, double height);

private:
  glm::dvec3 m_position;
  double m_width, m_height;

  AABB(glm::dvec3 m_position, double m_width, double m_height);
};