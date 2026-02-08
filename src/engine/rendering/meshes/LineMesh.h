#pragma once

#include "Mesh.h"

class LineMesh : public Mesh {
public:
  LineMesh() = default;

  void Draw() const override;

private:
  void SetupAttributes() const override;
};