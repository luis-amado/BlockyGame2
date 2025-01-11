#pragma once

#include <algorithm>
#include <osn/OpenSimplexNoise.h>

class Noise {
public:
  static double Noise2D(double x, double y, double offsetX, double offsetY, double scale, int octaves, double persistence, double lacunarity);
private:
  static OSN::Noise<2> s_noise;
};
