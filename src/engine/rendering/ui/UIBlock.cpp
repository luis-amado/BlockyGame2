#include "UIBlock.h"
#include <memory>
#include <algorithm>

UIBlock::UIBlock(UIProps props) : m_props(props) {}

void UIBlock::Draw() const {
  DrawMyself();
  for (const std::unique_ptr<UIBlock>& child : m_children) {
    child->Draw();
  }
}

void UIBlock::DrawMyself() const {}

// This function assumes it gets called from the parent element
void UIBlock::Update() {
  // This is the main logic for the UI layout algorithm
  // The key to this, is that it is not directly recursive but works in iterative passes

  // PASS 1: Sizes
  CalculateSize();

  // PASS 2: Positions
  m_position.x = m_props.x;
  m_position.y = m_props.y;
  CalculateChildrenPositions();
}

void UIBlock::SetProps(UIProps props) {
  m_props = props;
}

glm::vec2 UIBlock::GetPosition() const {
  return m_position;
}

glm::vec2 UIBlock::GetSize() const {
  return m_size;
}

void UIBlock::SetHeight(LengthValue height) {
  m_props.height = height;
}

void UIBlock::SetWidth(LengthValue width) {
  m_props.width = width;
}

void UIBlock::CalculateSize() {
  // Calculate size of children first
  for (const std::unique_ptr<UIBlock>& child : m_children) {
    child->CalculateSize();
  }

  if (m_props.width.GetType() == FIXED) {
    m_size.x = m_props.width.Get();
  }
  if (m_props.height.GetType() == FIXED) {
    m_size.y = m_props.height.Get();
  }

  if (m_props.width.GetType() != FIT && m_props.height.GetType() != FIT) {
    return;
  }

  int alongAxisLength = 0;
  int counterAxisLength = 0;
  if (m_props.axis == HORIZONTAL) {
    for (const std::unique_ptr<UIBlock>& child : m_children) {
      alongAxisLength += child->m_size.x;
      if (child->m_size.y > counterAxisLength) {
        counterAxisLength = child->m_size.y;
      }
    }

    if (m_props.width.GetType() == FIT)
      m_size.x = alongAxisLength + (m_children.size() - 1) * m_props.childGap + m_props.padding.Get(Padding::LEFT) + m_props.padding.Get(Padding::RIGHT);
    if (m_props.height.GetType() == FIT)
      m_size.y = counterAxisLength + m_props.padding.Get(Padding::TOP) + m_props.padding.Get(Padding::BOTTOM);
  } else {
    for (const std::unique_ptr<UIBlock>& child : m_children) {
      alongAxisLength += child->m_size.y;
      if (child->m_size.x > counterAxisLength) {
        counterAxisLength = child->m_size.x;
      }
    }

    if (m_props.height.GetType() == FIT)
      m_size.y = alongAxisLength + (m_children.size() - 1) * m_props.childGap + m_props.padding.Get(Padding::TOP) + m_props.padding.Get(Padding::BOTTOM);
    if (m_props.width.GetType() == FIT)
      m_size.x = counterAxisLength + m_props.padding.Get(Padding::LEFT) + m_props.padding.Get(Padding::RIGHT);
  }
}

void UIBlock::CalculateChildrenPositions() {
  int offset = 0;
  int widthOfChildren = 0;
  int heightOfChildren = 0;

  for (const std::unique_ptr<UIBlock>& child : m_children) {
    if (m_props.axis == HORIZONTAL) {
      widthOfChildren += child->m_size.x;
      heightOfChildren = std::max(heightOfChildren, (int)child->m_size.y);
    } else {
      widthOfChildren = std::max(widthOfChildren, (int)child->m_size.x);
      heightOfChildren += child->m_size.y;
    }
  }
  if (m_props.axis == HORIZONTAL) {
    widthOfChildren += m_props.childGap * (m_children.size() - 1);
  } else {
    heightOfChildren += m_props.childGap * (m_children.size() - 1);
  }

  for (const std::unique_ptr<UIBlock>& child : m_children) {
    if (m_props.horizontalAlignment == START) {
      child->m_position.x = m_position.x + child->m_props.x + m_props.padding.Get(Padding::LEFT) + offset * (m_props.axis == HORIZONTAL ? 1 : 0);
    } else if (m_props.horizontalAlignment == CENTER) {
      child->m_position.x = m_position.x + m_props.padding.Get(Padding::LEFT) + (m_size.x - m_props.padding.Get(Padding::LEFT) - m_props.padding.Get(Padding::RIGHT) - widthOfChildren) / 2 + child->m_props.x + offset * (m_props.axis == HORIZONTAL ? 1 : 0);
    } else {
      child->m_position.x = m_position.x + (m_size.x - widthOfChildren) + child->m_props.x - m_props.padding.Get(Padding::RIGHT) + offset * (m_props.axis == HORIZONTAL ? 1 : 0);
    }
    if (m_props.verticalAlignment == START) {
      child->m_position.y = m_position.y + child->m_props.y + m_props.padding.Get(Padding::TOP) + offset * (m_props.axis == VERTICAL ? 1 : 0);
    } else if (m_props.verticalAlignment == CENTER) {
      child->m_position.y = m_position.y + m_props.padding.Get(Padding::TOP) + (m_size.y - m_props.padding.Get(Padding::TOP) - m_props.padding.Get(Padding::BOTTOM) - heightOfChildren) / 2 + child->m_props.y + offset * (m_props.axis == VERTICAL ? 1 : 0);
    } else {
      child->m_position.y = m_position.y + (m_size.y - heightOfChildren) + child->m_props.y - m_props.padding.Get(Padding::BOTTOM) + offset * (m_props.axis == VERTICAL ? 1 : 0);
    }
    if (m_props.axis == HORIZONTAL) {
      offset += child->m_size.x + m_props.childGap;
    } else {
      offset += child->m_size.y + m_props.childGap;
    }
    child->CalculateChildrenPositions();
  }
}

LengthValue::LengthValue(int value) : m_type(FIXED), m_value(value) {}

LengthValue::LengthValue(LengthValueType type) : m_type(type), m_value(0) {}

int LengthValue::Get() const {
  return m_value;
}

LengthValueType LengthValue::GetType() const {
  return m_type;
}

Padding::Padding(int all) : m_values(all, all, all, all) {}

Padding::Padding(int vertical, int horizontal) : m_values(vertical, horizontal, vertical, horizontal) {}

Padding::Padding(int top, int right, int bottom, int left) : m_values(top, right, bottom, left) {}

int Padding::Get(PaddingDirection direction) const {
  return m_values[(int)direction];
}
