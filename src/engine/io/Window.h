#pragma once

#include <imgui/imgui.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <optional>
#include <glm/vec2.hpp>
#include <vector>

#include "util/ClassMacros.h"

class Window {
public:

  ONLY_MOVE_PTR(Window, m_handle);

  static std::optional<Window> CreateWindow(const std::string& windowTitle, int windowWidth, int windowHeight);
  ~Window();

  void ToggleFullscreen();
  GLFWwindow* GetHandle() const;
  ImGuiIO& GetImGuiIO() const;
  glm::ivec2 GetFrame() const;
  float GetAspectRatio() const;
  bool IsRunning() const;

  // Call this function before the game loop starts
  void Setup();
  // Call this function once at the start of each game loop iteration
  void BeginFrame() const;
  // Call this function once at the end of each game loop iteration
  void FinishFrame() const;

private:
  explicit Window(GLFWwindow* handle, int winWidth, int winHeight);

  GLFWwindow* m_handle;
  ImGuiIO* m_io;

  bool m_fullscreen = false;
  int m_prevWinXPos, m_prevWinYPos, m_prevMinWidth, m_prevWinHeight;

  friend void framebufferSizeCallback(GLFWwindow* windowHandle, int width, int height);
};