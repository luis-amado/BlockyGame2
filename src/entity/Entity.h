#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include "../physics/BoundingBox.h"
#include "util/threadsafe/ThreadSafeWrapper.h"
#include "../engine/rendering/Shader.h"

class Entity {
public:
  Entity(glm::dvec3 pos = { 0.0, 0.0, 0.0 }, glm::dvec2 rot = { 0.0, 0.0 });

  virtual BoundingBox GetBoundingBox() const = 0;
  virtual double GetEyeLevel() const = 0;

  void SetPosition(glm::dvec3 pos);
  void SetRotation(glm::dvec2 rot);
  glm::dvec3 GetPosition() const;
  const glm::dvec2& GetRotation() const;

  glm::dvec3 GetForwardVector() const;
  glm::dvec3 GetLookingVector() const;
  glm::dvec3 GetRightVector() const;
  glm::dvec3 GetRightVector(glm::dvec3 forwardVector) const;

private:
  ThreadSafeWrapper<glm::dvec3> m_pos;
  glm::dvec2 m_rot;
};