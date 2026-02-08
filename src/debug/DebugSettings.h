#pragma once

class DebugSettings {
public:
  // visual settings
  float defaultFOV = 90.0f;

  bool smoothLighting = true;

  // world updating settings
  bool updateWorld = true;
  int renderDistance = 16;
  int inMemoryBorder = 8;
  int terrainWorkerCount = 4;
  int lightingWorkerCount = 1;
  int meshWorkerCount = 4;

  // world visualization changes
  bool showChunkBoundaries = true;
  bool showPlayerHitbox = false;

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
  bool nightTime = false;

  // movement settings
  double gravity = -22.0;
  double jumpForce = 7.2;
  double walkSpeed = 3.5;
  double sprintMultiplier = 1.8;

  DebugSettings() = default;
  static DebugSettings instance;
};