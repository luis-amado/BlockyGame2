#pragma once

#include "UIQuad.h"
#include "../../textures/Texture.h"

class UITexturedQuad : public UIQuad {
public:
  UITexturedQuad() = default;
  UITexturedQuad(Texture* texture, UIProps props);

  void SetTexture(Texture* texture);
  void DrawMyself() const override;
private:
  Texture* m_texture = nullptr;
};