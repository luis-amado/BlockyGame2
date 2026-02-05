#pragma once

#include "Mesh.h"

class UIQuadMesh : public Mesh {
public:
  UIQuadMesh() = default;

private:
  void SetupAttributes() const override;
};