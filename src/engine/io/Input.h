#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <functional>

#include "Window.h"

using InputCallback = std::function<void(int key, int action, int mods)>;

#define KEY_SPACE GLFW_KEY_SPACE
#define KEY_LEFT_SHIFT GLFW_KEY_LEFT_SHIFT
#define KEY_RIGHT_SHIFT GLFW_KEY_RIGHT_SHIFT

#define MOUSE_BTN_LEFT GLFW_KEY_LAST+1
#define MOUSE_BTN_RIGHT GLFW_KEY_LAST+2
#define MOUSE_BTN_MIDDLE GLFW_KEY_LAST+3

class Input {
public:
  static double GetMouseDX();
  static double GetMouseDY();
  static double GetMouseDWheel();
  static bool IsCursorShown();

  static bool IsPressed(int key);
  static bool IsJustPressed(int key);
  static bool IsJustDoublePressed(int key);

  static void Init(Window* window);
  static void Reset();
  static void Resized();

  static void SubscribeKeyCallback(int key, InputCallback callback);
  static void SubscribeKeyCallbackRange(int startKey, int endKey, InputCallback callback);
  static void UnsubscribeKeyCallback(int key, InputCallback callback);
private:
  static double s_mouseDX, s_mouseDY, s_mouseDWheel;
  static double s_prevMouseX, s_prevMouseY;
  static bool s_framebufferTransition;

  static std::unordered_set<int> s_justPressedInputs;
  static std::unordered_map<int, std::vector<InputCallback>> s_inputCallbacks;
  static std::unordered_map<int, float> s_timeSinceLastInput;

  static bool s_cursorShown;

  static Window* s_window;

  friend void mousePositionCallback(GLFWwindow* window, double xpos, double ypos);
  friend void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
  friend void scrollCallback(GLFWwindow* window, double xoff, double yoff);
  friend void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
};