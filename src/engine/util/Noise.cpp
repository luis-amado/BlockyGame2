#include "Noise.h"

OSN::Noise<2> Noise::s_noise2;
OSN::Noise<3> Noise::s_noise3;

double Noise::Noise2D(double x, double y, double offsetX, double offsetY, double scale, int octaves, double persistence, double lacunarity) {
  double total = 0.0;

  double frequency = 1.0;
  double amplitude = 1.0;
  double totalAmplitude = 0.0;

  for (int i = 0; i < octaves; i++) {
    total += s_noise2.eval((x + offsetX) * frequency / scale, (y + offsetY) * frequency / scale) * amplitude;
    totalAmplitude += amplitude;
    amplitude *= persistence;
    frequency *= lacunarity;
  }
  return total / totalAmplitude;
}

double Noise::Noise3D(double x, double y, double z, double offsetX, double offsetY, double offsetZ, double scale) {
  return s_noise3.eval((x + offsetX) / scale, (y + offsetY) / scale, (z + offsetZ) / scale);
}

double Noise::RandomNoise2D(double x, double y, double offsetX, double offsetY) {
  const int PRIME = 73856093;
  int xi = static_cast<int32_t>(std::floor(x + offsetX * 10000)) + PRIME;
  int yi = static_cast<int32_t>(std::floor(y + offsetY * 10000)) + PRIME;

  unsigned int h = HashCoord(xi, yi);
  return (h & 0xFFFFFF) / 16777215.0f; // Normalize to [0,1]
}

unsigned int Noise::HashCoord(int x, int y) {
  const uint32_t PRIME1 = 0x85ebca6b;
  const uint32_t PRIME2 = 0xc2b2ae35;

  x *= PRIME1;
  x ^= (x >> 15);
  x *= PRIME2;

  y *= PRIME2;
  y ^= (y >> 15);
  y *= PRIME1;

  uint32_t h = x ^ (y * 0x27d4eb2d);
  return h;
}
