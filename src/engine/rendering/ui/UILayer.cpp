#include "UILayer.h"

void UILayer::Resize(glm::ivec2 windowSize) {
  SetWidth(windowSize.x);
  SetHeight(windowSize.y);
}