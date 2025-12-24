#pragma once

#include <cmath>

#define XYZ(vector) vector.x, vector.y, vector.z

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

inline bool IsPowerOf2(int x) {
  return x > 0 && (x & (x - 1)) == 0;
}

inline long long NearestPowerOf2(long long n) {
  long long a = std::log2(n);

  if (std::pow(2, a) == n) {
    return n;
  } else {
    return std::pow(2, a + 1);
  }
}

inline int IntLerp(int a, int b, double t) {
  return a + (b - a) * t;
}

template <typename T>
inline T Lerp(T a, T b, T t) {
  return a + (b - a) * t;
}

template <typename T>
inline T MoveTowards(T current, T target, T distance) {
  if (target - current > 0.0) {
    return std::min(current + std::abs(distance), target);
  } else {
    return std::max(current - std::abs(distance), target);
  }
}

inline int Sign(float v) {
  if (v > 0.0f) return 1.0f;
  if (v < 0.0f) return -1.0f;
  return 0.0f;
}

template <typename T>
inline T Clamp(T value, T min, T max) {
  return std::max(std::min(value, max), min);
}

} // namespace MathUtil