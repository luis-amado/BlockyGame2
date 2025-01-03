#pragma once

class DebugSettings {
public:
  bool updateWorld = true;
  int renderDistance = 12;

  DebugSettings() = default;
  static DebugSettings instance;
};