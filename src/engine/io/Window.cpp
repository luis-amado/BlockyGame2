#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
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

  // Get window stats
  const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
  LOG(INFO) << "Window refresh rate: " << mode->refreshRate << " hz";

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
  glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

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

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
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

ImGuiIO& Window::GetImGuiIO() const {
  return *m_io;
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

void Window::Setup() {
  Time::Start();

  // SETUP IMGUI
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  m_io = &ImGui::GetIO(); (void)*m_io;
  ImGui_ImplGlfw_InitForOpenGL(m_handle, true);
  ImGui_ImplOpenGL3_Init("#version 330");
}

void Window::BeginFrame() const {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  Time::Update();

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void Window::FinishFrame() const {
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  glfwSwapBuffers(m_handle);
  Input::Reset();
  glfwPollEvents();
}