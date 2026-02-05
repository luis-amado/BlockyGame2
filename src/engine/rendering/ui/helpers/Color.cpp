#include "Color.h"

Color Color::WHITE = Color::RGB(1.0f, 1.0f, 1.0f);
Color Color::BLACK = Color::RGB(0.0f, 0.0f, 0.0f);
Color Color::RED = Color::RGB(1.0f, 0.0f, 0.0f);
Color Color::GREEN = Color::RGB(0.0f, 1.0f, 0.0f);
Color Color::BLUE = Color::RGB(0.0f, 0.0f, 1.0f);

Color Color::RGB(float r, float g, float b) {
  return Color(r, g, b, 1.0f);
}

Color Color::RGBA(float r, float g, float b, float a) {
  return Color(r, g, b, a);
}

Color::Color(float r, float g, float b, float a) : m_rgba(r, g, b, a) {}

glm::vec4 Color::GetRGBA() const {
  return m_rgba;
}

Color Color::Opacity(float opacity) const {
  return Color(m_rgba.r, m_rgba.g, m_rgba.b, opacity);
}
