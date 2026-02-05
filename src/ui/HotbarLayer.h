#pragma once

#include "rendering/ui/UILayer.h"
#include "rendering/ui/drawables/UIColoredQuad.h"
#include "../init/Blocks.h"
#include "../entity/PlayerEntity.h"

class HotbarSlot : public UIColoredQuad {
public:
  HotbarSlot(const Block& block, int index);

  Color GetSelectedColor() const;
  Color GetUnselectedColor() const;

  void SetSelectedIndex(int selectedIndex);
private:

  const Block& m_block;
  Texture m_texture;
  int m_index;
};

class HotbarLayer : public UILayer {
public:
  HotbarLayer(PlayerEntity& player);

private:
  std::vector<HotbarSlot*> m_slots;
};