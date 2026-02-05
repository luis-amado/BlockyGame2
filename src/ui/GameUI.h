#pragma once

#include <glm/mat4x4.hpp>
#include "rendering/textures/Texture.h"
#include "rendering/ui/drawables/UITexturedQuad.h"
#include "rendering/ui/drawables/UIColoredQuad.h"
#include "io/Window.h"
#include "HotbarLayer.h"
#include "../entity/PlayerEntity.h"

class GameUI {
public:
  GameUI(PlayerEntity& player);

  void Update(const Window& window);
  void Draw();

private:
  HotbarLayer m_hotbarLayer;
  UILayer m_crosshairLayer;
  Texture m_crosshairTexture;
};