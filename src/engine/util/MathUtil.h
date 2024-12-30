#pragma once

namespace MathUtil {

int Mod(int a, int b) {
  return (b + (a % b)) % b;
}

} // namespace MathUtil