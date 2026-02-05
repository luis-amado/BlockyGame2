#pragma once

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <vector>

enum AlignmentType { START, CENTER, END };
enum DirectionType { HORIZONTAL, VERTICAL };
enum LengthValueType {
  FIXED, FIT, /*FILL*/
};

class LengthValue {
public:
  LengthValue(int value);
  LengthValue(LengthValueType type);

  int Get() const;
  LengthValueType GetType() const;
private:
  LengthValueType m_type;
  int m_value;
};

class Padding {
public:
  Padding(int all);
  Padding(int vertical, int horizontal);
  Padding(int top, int right, int bottom, int left);

  enum PaddingDirection { TOP, RIGHT, BOTTOM, LEFT };

  int Get(PaddingDirection direction) const;
private:
  glm::ivec4 m_values;
};

struct UIProps {
  int x = 0;
  int y = 0;
  LengthValue width = FIT;
  LengthValue height = FIT;
  Padding padding = { 0, 0, 0, 0 };
  int childGap = 0;
  DirectionType axis = HORIZONTAL;
  AlignmentType horizontalAlignment = START;
  AlignmentType verticalAlignment = START;
};

// A UIBlock is a renderable UI object that may have children
// In the future, a UITextBlock class can extend this class to describe a block of text
class UIBlock {
public:
  virtual ~UIBlock() {}

  UIBlock() = default;
  UIBlock(UIProps props);

  void Draw() const;
  virtual void DrawMyself() const;

  // Reposition and resize itself and child elements
  void Update();

  // Takes control of the provided pointer
  template <typename T>
  T& AddChild(std::unique_ptr<T> child) {
    child->m_parent = this;
    m_children.push_back(std::move(child));
    return (T&)*m_children[m_children.size() - 1];
  }

  // Creates a new unique_ptr for the child element
  template <typename T, typename... Args>
  T& NewChild(Args... args) {
    m_children.push_back(std::make_unique<T>(args...));
    return (T&)*m_children[m_children.size() - 1];
  }

  void SetProps(UIProps props);

  glm::vec2 GetPosition() const;
  glm::vec2 GetSize() const;

  // Change UIElementProperties individually
  void SetWidth(LengthValue width);
  void SetHeight(LengthValue height);

private:

  void CalculateSize();
  void CalculateChildrenPositions();

  std::vector<std::unique_ptr<UIBlock>> m_children;
  UIProps m_props;

  glm::vec2 m_position;
  glm::vec2 m_size;
};