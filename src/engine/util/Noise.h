#pragma once

#include <algorithm>
#include <osn/OpenSimplexNoise.h>

class Noise {
public:
  static double Noise2D(double x, double y, double offsetX, double offsetY, double scale, int octaves, double persistence, double lacunarity);
  static double Noise3D(double x, double y, double z, double offsetX, double offsetY, double offsetZ, double scale);
private:
  static OSN::Noise<2> s_noise2;
  static OSN::Noise<3> s_noise3;
};
