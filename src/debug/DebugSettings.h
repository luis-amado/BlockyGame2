#pragma once

class DebugSettings {
public:
  // visual settings
  float fov = 90.0f;

  // world updating settings
  bool updateWorld = true;
  int renderDistance = 16;
  int terrainWorkerCount = 4;
  int lightingWorkerCount = 1;
  int meshWorkerCount = 4;

  // world visualization changes
  float chunkSplit = 0.0f;

  // terrain generation settings
  double noiseScale = 100.0;
  int octaves = 5;
  double persistence = 0.6;
  double lacunarity = 1.8;
  float noiseOffsets[2] = { 0.0, 0.0 };

  double coalThreshold = 0.15;
  double coalScale = 10.0;

  double ironThreshold = 0.15;
  double ironScale = 10.0;

  // cave settings
  double caveNoiseScale = 30.0;
  double caveThreshold = 0.25;
  float caveNoiseOffsets[3] = { 0.0, 0.0, 0.0 };

  int baseTerrainHeight = 60;
  int terrainRange[2] = { -10, 80 };

  bool nightVision = false;

  DebugSettings() = default;
  static DebugSettings instance;
};