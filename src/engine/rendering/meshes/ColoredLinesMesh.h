#pragma once

#include "Mesh.h"

class ColoredLinesMesh : public Mesh {
public:
  ColoredLinesMesh() = default;

  void Draw() const override;
private:
  void SetupAttributes() const override;
};