#pragma once

#include "Entity.h"
#include <glm/vec3.hpp>
#include "../world/World.h"

class PhysicsEntity : public Entity {
public:
  PhysicsEntity(glm::dvec3 pos = { 0.0, 0.0, 0.0 }, glm::dvec2 rot = { 0.0, 0.0 });

  virtual double GetGravity() const;
  virtual double GetJumpForce() const;
  const glm::dvec3& GetVelocity() const;

  void SetDisablePhysics(bool disablePhysics);
  void PhysicsUpdate(const World& world);

protected:
  glm::dvec3 m_velocity;

  void Jump();

private:
  bool m_disablePhysics = false;
  bool m_grounded = false;
};