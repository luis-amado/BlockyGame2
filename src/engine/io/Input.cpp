#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Input.h"
#include "Window.h"
#include "util/Logging.h"

double Input::s_mouseDX = 0.0;
double Input::s_mouseDY = 0.0;
double Input::s_mouseDWheel = 0.0;
double Input::s_prevMouseX = 0.0;
double Input::s_prevMouseY = 0.0;
bool Input::s_framebufferTransition = false;

bool Input::s_cursorShown = false;

Window* Input::s_window = nullptr;

// Callback implementations
void mousePositionCallback(GLFWwindow* window, double xpos, double ypos) {

  // Prevent cursor position change from affecting position offset after a resize
  // This is because there can be jumps when the framebuffer resizes
  if (Input::s_framebufferTransition) {
    Input::s_framebufferTransition = false;
    Input::s_prevMouseX = xpos;
    Input::s_prevMouseY = ypos;
    return;
  }

  Input::s_mouseDX += xpos - Input::s_prevMouseX;
  Input::s_mouseDY += ypos - Input::s_prevMouseY;

  Input::s_prevMouseX = xpos;
  Input::s_prevMouseY = ypos;
}

void scrollCallback(GLFWwindow* window, double xoff, double yoff) {
  Input::s_mouseDWheel += yoff - xoff;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  // CMD + W: Closes the window
  if (mods & GLFW_MOD_SUPER && key == GLFW_KEY_W && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
  // F11: Toggles Fullscreen
  if (key == GLFW_KEY_F11 && action == GLFW_PRESS) {
    Input::s_window->ToggleFullscreen();
  }
  // ESC: Toggles mouse cursor
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    if (Input::s_cursorShown) {
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
      glm::ivec2 windowFrame = Input::s_window->GetFrame();
      int midX = windowFrame.x / 4;
      int midY = windowFrame.y / 4;
      glfwSetCursorPos(window, midX, midY);
      Input::s_prevMouseX = midX;
      Input::s_prevMouseY = midY;
    }
    Input::s_cursorShown = !Input::s_cursorShown;
  }
}

double Input::GetMouseDX() {
  return s_mouseDX;
}

double Input::GetMouseDY() {
  return s_mouseDY;
}

double Input::GetMouseDWheel() {
  return s_mouseDWheel;
}

bool Input::IsCursorShown() {
  return s_cursorShown;
}

void Input::Init(Window* window) {
  s_window = window;

  // Set the initial mouse position
  glfwGetCursorPos(window->GetHandle(), &s_prevMouseX, &s_prevMouseY);

  // Setup input callbacks
  glfwSetCursorPosCallback(window->GetHandle(), mousePositionCallback);
  glfwSetKeyCallback(window->GetHandle(), keyCallback);
  glfwSetScrollCallback(window->GetHandle(), scrollCallback);

  // Hide and lock cursor
  glfwSetCursorPos(window->GetHandle(), 0, 0);
  glfwSetInputMode(window->GetHandle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  Reset();
}

void Input::Reset() {
  s_mouseDX = 0.0;
  s_mouseDY = 0.0;
  s_mouseDWheel = 0.0;
}

void Input::Resized() {
  s_framebufferTransition = true;
}
