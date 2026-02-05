#pragma once

#include "UIBlock.h"

class UILayer : public UIBlock {
public:
  void Resize(glm::ivec2 windowSize);
};