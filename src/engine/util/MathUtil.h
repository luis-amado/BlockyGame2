#pragma once

#include <cmath>

namespace MathUtil {

inline int Mod(int a, int b) {
  return (b + (a % b)) % b;
}

inline float fMod(float a, float b) {
  return std::fmodf((b + (std::fmodf(a, b))), b);
}

inline double Map(double x, double inStart, double inEnd, double outStart, double outEnd) {
  return outStart + ((outEnd - outStart) / (inEnd - inStart)) * (x - inStart);
}

inline int FloorToInt(double x) {
  return (int)std::floor(x);
}

} // namespace MathUtil