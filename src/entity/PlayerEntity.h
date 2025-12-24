#pragma once

#include "PhysicsEntity.h"
#include "../physics/BoundingBox.h"
#include <glm/mat4x4.hpp>
#include "../debug/DebugSettings.h"
#include <optional>
#include <limits>

class PlayerEntity : public PhysicsEntity {

public:
  PlayerEntity();

  glm::mat4 GetFirstPersonViewMatrix();
  BoundingBox GetBoundingBox() const override;
  double GetEyeLevel() const override;
  double GetSpeedMultiplier() const;

  // Special method for player entities that runs once per frame
  void Update(World& world);

  float GetFOVChange() const;
  std::optional<float> GetFOVOverride() const;
  bool CanFly() const;
  void UpdateLookingAt(const World& world);
  const std::optional<glm::ivec3>& GetLookingAtBlock() const;


private:
  double m_walkSpeed = DebugSettings::instance.walkSpeed;
  double m_sprintMultiplier = DebugSettings::instance.sprintMultiplier;
  double m_speedMultiplier = 1.0;

  bool m_flying = false;
  bool m_ghost = false;
  bool m_sprinting = false;
  bool m_zoomed = false;

  double m_timeSinceLastAutoJump = std::numeric_limits<double>::max();
  std::optional<glm::ivec3> m_lookingAtBlock;
  std::optional<glm::ivec3> m_placingAtBlock;

  // Used for smoothing face movements
  glm::dvec2 m_rotationVelocity;

};