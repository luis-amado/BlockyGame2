#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Window.h"

class Input {
public:
  static double GetMouseDX();
  static double GetMouseDY();
  static double GetMouseDWheel();
  static bool IsCursorShown();

  static void Init(Window* window);
  static void Reset();
  static void Resized();
private:
  static double s_mouseDX, s_mouseDY, s_mouseDWheel;
  static double s_prevMouseX, s_prevMouseY;
  static bool s_framebufferTransition;

  static bool s_cursorShown;

  static Window* s_window;

  friend void mousePositionCallback(GLFWwindow* window, double xpos, double ypos);
  friend void scrollCallback(GLFWwindow* window, double xoff, double yoff);
  friend void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
};