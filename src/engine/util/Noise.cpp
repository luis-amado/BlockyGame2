#include "Noise.h"

OSN::Noise<2> Noise::s_noise;

double Noise::Noise2D(double x, double y, double offsetX, double offsetY, double scale, int octaves, double persistence, double lacunarity) {
  double total = 0.0;

  double frequency = 1.0;
  double amplitude = 1.0;
  double totalAmplitude = 0.0;

  for (int i = 0; i < octaves; i++) {
    total += s_noise.eval((x + offsetX) * frequency / scale, (y + offsetY) * frequency / scale) * amplitude;
    totalAmplitude += amplitude;
    amplitude *= persistence;
    frequency *= lacunarity;
  }
  return total / totalAmplitude;
}