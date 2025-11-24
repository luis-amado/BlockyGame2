#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <functional>

#include "Window.h"

using KeyCallback = std::function<void(int action, int mods)>;

class Input {
public:
  static double GetMouseDX();
  static double GetMouseDY();
  static double GetMouseDWheel();
  static bool IsCursorShown();

  static bool IsKeyPressed(int key);
  static bool IsKeyJustPressed(int key);

  static void Init(Window* window);
  static void Reset();
  static void Resized();

  static void SubscribeKeyCallback(int key, KeyCallback callback);
  static void UnsubscribeKeyCallback(int key, KeyCallback callback);
private:
  static double s_mouseDX, s_mouseDY, s_mouseDWheel;
  static double s_prevMouseX, s_prevMouseY;
  static bool s_framebufferTransition;

  static std::unordered_set<int> s_justPressedKeys;
  static std::unordered_map<int, std::vector<KeyCallback>> s_keyCallbacks;

  static bool s_cursorShown;

  static Window* s_window;

  friend void mousePositionCallback(GLFWwindow* window, double xpos, double ypos);
  friend void scrollCallback(GLFWwindow* window, double xoff, double yoff);
  friend void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
};