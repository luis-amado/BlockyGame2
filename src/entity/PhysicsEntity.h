#pragma once

#include "Entity.h"

class PhysicsEntity : public Entity {
public:
  PhysicsEntity(glm::dvec3 pos = { 0.0, 0.0, 0.0 }, glm::dvec2 rot = { 0.0, 0.0 });

  virtual double GetGravity() const;
  virtual double GetJumpForce() const;

  void SetDisablePhysics(bool disablePhysics);

private:
  bool m_disablePhysics = false;
};