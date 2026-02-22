#include "Color.h"
#include "util/DebugMacros.h"

Color Color::WHITE = Color::RGB(1.0f, 1.0f, 1.0f);
Color Color::BLACK = Color::RGB(0.0f, 0.0f, 0.0f);
Color Color::RED = Color::RGB(1.0f, 0.0f, 0.0f);
Color Color::GREEN = Color::RGB(0.0f, 1.0f, 0.0f);
Color Color::BLUE = Color::RGB(0.0f, 0.0f, 1.0f);

namespace {

int HexDigitToInt(char c) {
  if (c >= '0' && c <= '9') return c - '0';
  return c - 'A' + 10;
}

}  // namespace

Color::Color(std::string hexCode) {
  DEBUG_ASSERT(hexCode[0] == '#') << "Provided invalid hexcode";

  // Validate and normalize digits
  for (int i = 1; i < hexCode.length(); i++) {
    if (hexCode[i] >= '0' && hexCode[i] <= '9') continue;
    if (hexCode[i] >= 'A' && hexCode[i] <= 'F') continue;
    if (hexCode[i] >= 'a' && hexCode[i] <= 'f') {
      hexCode[i] -= 'a' - 'A'; // convert to uppercase
      continue;
    }

    // character was invalid
    DEBUG_LOG(ERROR) << "Provided invalid hexcode";
    return;
  }

  unsigned int r, g, b, a = 255;

  if (hexCode.length() == 4) {
    // RGB
    r = HexDigitToInt(hexCode[1]) * 16 + HexDigitToInt(hexCode[1]);
    g = HexDigitToInt(hexCode[2]) * 16 + HexDigitToInt(hexCode[2]);
    b = HexDigitToInt(hexCode[3]) * 16 + HexDigitToInt(hexCode[3]);

  } else if (hexCode.length() == 7) {
    // RRGGBB
    r = HexDigitToInt(hexCode[1]) * 16 + HexDigitToInt(hexCode[2]);
    g = HexDigitToInt(hexCode[3]) * 16 + HexDigitToInt(hexCode[4]);
    b = HexDigitToInt(hexCode[5]) * 16 + HexDigitToInt(hexCode[6]);

  } else if (hexCode.length() == 5) {
    // RGBA
    r = HexDigitToInt(hexCode[1]) * 16 + HexDigitToInt(hexCode[1]);
    g = HexDigitToInt(hexCode[2]) * 16 + HexDigitToInt(hexCode[2]);
    b = HexDigitToInt(hexCode[3]) * 16 + HexDigitToInt(hexCode[3]);
    a = HexDigitToInt(hexCode[4]) * 16 + HexDigitToInt(hexCode[4]);

  } else if (hexCode.length() == 9) {
    // RRGGBBAA
    r = HexDigitToInt(hexCode[1]) * 16 + HexDigitToInt(hexCode[2]);
    g = HexDigitToInt(hexCode[3]) * 16 + HexDigitToInt(hexCode[4]);
    b = HexDigitToInt(hexCode[5]) * 16 + HexDigitToInt(hexCode[6]);
    a = HexDigitToInt(hexCode[7]) * 16 + HexDigitToInt(hexCode[8]);

  } else {
    DEBUG_LOG(ERROR) << "Provided invalid hexcode";
  }

  m_rgba = glm::vec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
}

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
