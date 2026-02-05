#pragma once

#include "../helpers/Color.h"
#include "UIQuad.h"

class UIColoredQuad : public UIQuad {
public:
  UIColoredQuad() = default;
  UIColoredQuad(Color color, UIProps props);

  void SetColor(Color color);

  void DrawMyself() const override;
private:
  Color m_color = Color::WHITE;
};