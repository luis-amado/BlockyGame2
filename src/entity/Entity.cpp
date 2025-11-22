#include "Entity.h"
#include <glm/trigonometric.hpp>
#include <glm/gtc/matrix_transform.hpp>

Entity::Entity(glm::dvec3 pos, glm::dvec2 rot)
  : m_pos(pos), m_rot(rot) {}

void Entity::SetPosition(glm::dvec3 pos) {
  m_pos.Set(pos);
}

void Entity::SetRotation(glm::dvec2 rot) {
  m_rot = rot;
}

glm::dvec3 Entity::GetPosition() const {
  return m_pos.Get();
}

const glm::dvec2& Entity::GetRotation() const {
  return m_rot;
}

glm::dvec3 Entity::GetForwardVector() const {
  return {
    std::sin(glm::radians(m_rot.y)),
    0,
    -std::cos(glm::radians(m_rot.y))
  };
}

glm::dvec3 Entity::GetLookingVector() const {
  return {
    std::sin(glm::radians(m_rot.y)) * std::cos(glm::radians(m_rot.x)),
    -std::sin(glm::radians(m_rot.x)),
    -std::cos(glm::radians(m_rot.y)) * std::cos(glm::radians(m_rot.x))
  };
}

glm::dvec3 Entity::GetRightVector() const {
  return {
    std::sin(glm::radians(m_rot.y + 90.0)),
    0,
    -std::cos(glm::radians(m_rot.y + 90.0))
  };
}

glm::dvec3 Entity::GetRightVector(glm::dvec3 forwardVector) const {
  return glm::cross(forwardVector, { 0, 1, 0 });
}



