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
