#pragma once

#include "PhysicsEntity.h"
#include "../physics/BoundingBox.h"
#include <glm/mat4x4.hpp>
#include "../debug/DebugSettings.h"

class PlayerEntity : public PhysicsEntity {

public:
  PlayerEntity();

  glm::mat4 GetFirstPersonViewMatrix();
  BoundingBox GetBoundingBox() const override;
  double GetEyeLevel() const override;

  // Special method for player entities that runs once per frame
  void Update();

  // returns a percentage change to FOV
  float GetFOVChange() const;
  bool CanFly() const;


private:
  double m_walkSpeed = DebugSettings::instance.walkSpeed;
  double m_sprintSpeed = DebugSettings::instance.sprintSpeed;
  bool m_flying = false;
  bool m_sprinting = false;

};