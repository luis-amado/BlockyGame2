#pragma once

#include <imgui/imgui.h>
#include "../world/World.h"
#include "../engine/camera/Camera.h"
#include "../engine/io/Window.h"

class DebugInformation {

public:
  static void ShowIfActive(World& world, const Camera& camera);
  static void Setup(const Window& window);
  static void Toggle();

private:
  static bool s_showDebugInformation;
  static bool s_showDebugPanel;
  static ImFont* s_font;

  static int s_fps;
  static double s_lastFPSCheckTime;
  static double s_currTime;
};