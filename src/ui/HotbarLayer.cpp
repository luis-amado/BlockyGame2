#include "HotbarLayer.h"

#include "rendering/ui/drawables/UITexturedQuad.h"
#include "util/Logging.h"

HotbarSlot::HotbarSlot(const Block& block, int index)
  : UIColoredQuad(GetUnselectedColor(), UIProps {
    .padding = 2,
    .horizontalAlignment = CENTER,
    .verticalAlignment = CENTER,
    }), m_block(block), m_texture(block.GetTextures().north), m_index(index) {

  NewChild<UIColoredQuad>(Color::BLACK, UIProps {
    .padding = 4,
    .horizontalAlignment = CENTER,
    .verticalAlignment = CENTER,
    }).NewChild<UITexturedQuad>(&m_texture, UIProps {
      .width = 48,
      .height = 48,
      });
}

Color HotbarSlot::GetSelectedColor() const {
  return Color::WHITE.Opacity(0.8f);
}

Color HotbarSlot::GetUnselectedColor() const {
  return Color::BLACK.Opacity(0.0f);
}

void HotbarSlot::SetSelectedIndex(int selectedIndex) {
  if (m_index == selectedIndex) {
    SetColor(GetSelectedColor());
  } else {
    SetColor(GetUnselectedColor());
  }
}

HotbarLayer::HotbarLayer(PlayerEntity& player) {
  SetProps({
    .horizontalAlignment = CENTER,
    .verticalAlignment = END,
    .padding = {20, 0},
    });

  UIColoredQuad& hotbarContainer = NewChild<UIColoredQuad>(Color::BLACK.Opacity(0.3f), UIProps {
    .childGap = 2,
    .padding = 2,
    });

  std::vector<const Block*> blocks = PlayerEntity::GetPlaceableBlocks();
  int i = 0;
  for (const Block* block : blocks) {
    m_slots.push_back(&hotbarContainer.NewChild<HotbarSlot>(*block, i++));
  }

  player.SetOnSelectedSlotChanged([&](int newSlot) {
    for (HotbarSlot* slot : m_slots) {
      slot->SetSelectedIndex(newSlot);
    }
  });
}

