#pragma once

#include <cmath>

namespace MathUtil {

inline int Mod(int a, int b) {
  return (b + (a % b)) % b;
}

inline float fMod(float a, float b) {
  return std::fmodf((b + (std::fmodf(a, b))), b);
}

} // namespace MathUtil