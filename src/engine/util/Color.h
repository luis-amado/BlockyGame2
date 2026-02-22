#pragma once

#include <glm/vec4.hpp>
#include <string>

#define COLOR_RGB(color) (color).GetRGBA().r, (color).GetRGBA().g, (color).GetRGBA().b

class Color {
public:
  Color() = default;
  Color(std::string hexCode);

  static Color RGB(float r, float g, float b);
  static Color RGBA(float r, float g, float b, float a);

  static Color WHITE;
  static Color BLACK;
  static Color RED;
  static Color GREEN;
  static Color BLUE;

  glm::vec4 GetRGBA() const;
  Color Opacity(float opacity) const;
private:
  Color(float r, float g, float b, float a);

  glm::vec4 m_rgba;

};