#include "PhysicsEntity.h"

PhysicsEntity::PhysicsEntity(glm::dvec3 pos, glm::dvec2 rot)
  : Entity(pos, rot) {}

double PhysicsEntity::GetGravity() const {
  return -9.81;
}

double PhysicsEntity::GetJumpForce() const {
  return 10.0;
}

void PhysicsEntity::SetDisablePhysics(bool disablePhysics) {
  m_disablePhysics = disablePhysics;
}