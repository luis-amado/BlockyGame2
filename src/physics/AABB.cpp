#include "AABB.h"

#include <glm/vec3.hpp>

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