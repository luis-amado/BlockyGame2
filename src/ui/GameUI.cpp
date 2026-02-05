#include "GameUI.h"

#include <glm/gtc/matrix_transform.hpp>
#include "rendering/ShaderLibrary.h"

GameUI::GameUI(PlayerEntity& player) : m_hotbarLayer(player), m_crosshairTexture("ui/crosshair") {
  m_crosshairLayer.SetProps({
    .horizontalAlignment = CENTER,
    .verticalAlignment = CENTER,
    });

  m_crosshairLayer.NewChild<UITexturedQuad>(&m_crosshairTexture, UIProps {
    .width = 45,
    .height = 45
    });
}

void GameUI::Update(const Window& window) {
  glm::ivec2 windowSize = window.GetSize();
  glm::mat4 projection2D = glm::ortho(0.0f, (float)windowSize.x, (float)windowSize.y, 0.0f);
  ShaderLibrary::GetInstance().LoadMatrix4f("projection2D", projection2D);

  m_hotbarLayer.Resize(windowSize);
  m_crosshairLayer.Resize(windowSize);

  m_hotbarLayer.Update();
  m_crosshairLayer.Update();
}

void GameUI::Draw() {
  glDisable(GL_DEPTH_TEST);

  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);
  m_crosshairLayer.Draw();

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  m_hotbarLayer.Draw();

  glDisable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
}
