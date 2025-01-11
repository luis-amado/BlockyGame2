#pragma once

class DebugSettings {
public:
  // world updating settings
  bool updateWorld = true;
  int renderDistance = 12;
  int terrainWorkerCount = 2;
  int meshWorkerCount = 4;

  // terrain generation settings
  double noiseScale = 100.0;
  int octaves = 5;
  double persistence = 0.6;
  double lacunarity = 1.8;
  float noiseOffsets[2] = { 0.0, 0.0 };

  int baseTerrainHeight = 60;
  int terrainRange[2] = { -10, 80 };

  DebugSettings() = default;
  static DebugSettings instance;
};