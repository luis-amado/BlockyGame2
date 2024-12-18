#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Window.h"
#include "Input.h"
#include "util/Logging.h"
#include "Time.h"

void framebufferSizeCallback(GLFWwindow* windowHandle, int width, int height) {
  glViewport(0, 0, width, height);
  Input::Resized();
}

std::optional<Window> Window::CreateWindow(const std::string& windowTitle, int windowWidth, int windowHeight) {

  if (!glfwInit()) {
    LOG(ERROR) << "Failed to initialize GLFW";
    return std::nullopt;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

  GLFWwindow* windowHandle = glfwCreateWindow(windowWidth, windowHeight, windowTitle.c_str(), NULL, NULL);

  if (windowHandle == NULL) {
    LOG(ERROR) << "Failed to create window";
    glfwTerminate();
    return std::nullopt;
  }

  glfwMakeContextCurrent(windowHandle);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    LOG(ERROR) << "Failed to initialize GLAD";
    glfwTerminate();
    return std::nullopt;
  }

  // Setup user pointer
  Window window = Window(windowHandle, windowWidth, windowHeight);
  glfwSetWindowUserPointer(windowHandle, &window);

  // Setup window callbacks
  glfwSetFramebufferSizeCallback(windowHandle, framebufferSizeCallback);

  glClearColor(0.0, 0.0, 0.0, 1.0);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  glfwSwapInterval(1);

  return window;
}

Window::Window(GLFWwindow* handle, int winWidth, int winHeight)
  : m_handle(handle), m_prevMinWidth(winWidth), m_prevWinHeight(winHeight) {}

Window::~Window() {
  if (m_handle != nullptr) {
    glfwDestroyWindow(m_handle);
    glfwTerminate();
  }
}

void Window::ToggleFullscreen() {
  if (m_fullscreen) {
    // Toggle fullscreen off
    glfwSetWindowMonitor(m_handle, NULL, m_prevWinXPos, m_prevWinYPos, m_prevMinWidth, m_prevWinHeight, 0);
  } else {
    // Toggle fullscreen on
    glfwGetWindowSize(m_handle, &m_prevMinWidth, &m_prevWinHeight);
    glfwGetWindowPos(m_handle, &m_prevWinXPos, &m_prevWinYPos);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    glfwSetWindowMonitor(m_handle, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
  }
  m_fullscreen = !m_fullscreen;
}

GLFWwindow* Window::GetHandle() const {
  return m_handle;
}

glm::ivec2 Window::GetFrame() const {
  glm::ivec2 frame;
  glfwGetFramebufferSize(m_handle, &frame.x, &frame.y);
  return frame;
}

float Window::GetAspectRatio() const {
  int width, height;
  glfwGetFramebufferSize(m_handle, &width, &height);
  return (float)width / height;
}

bool Window::IsRunning() const {
  return !glfwWindowShouldClose(m_handle);
}

void Window::Setup() const {
  Time::Start();
}

void Window::BeginFrame() const {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  Time::Update();
}

void Window::FinishFrame() const {
  glfwSwapBuffers(m_handle);
  Input::Reset();
  glfwPollEvents();
}