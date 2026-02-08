#pragma once

#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include "../../meshes/UIQuadMesh.h"
#include "../UIBlock.h"

class UIQuad : public UIBlock {
public:
  UIQuad() = default;
  UIQuad(UIProps props);

  virtual void DrawMyself() const override;
protected:
  UIQuadMesh& GetQuadMesh() const;
  glm::mat4 GetModelMatrix() const;
};