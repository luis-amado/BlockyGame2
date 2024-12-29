#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>

#include "util/Logging.h"
#include "util/OptionalMacros.h"
#include "engine/rendering/Shader.h"
#include "engine/rendering/Texture.h"
#include "engine/rendering/Mesh.h"
#include "engine/camera/Camera.h"
#include "engine/io/Window.h"
#include "engine/io/Input.h"
#include "engine/io/Time.h"

#include "world/Chunk.h"

const int SCR_WIDTH = 800;
const int SCR_HEIGHT = 800;
const float FOV = 70.0f;

bool showDebugInfo = false;

glm::mat4 projection;

int main() {
  // Logger::showLogsFile();
  Logger::setLogLevel(INFO);

  ASSIGN_OR_DIE(Window window, Window::CreateWindow("LuisCraft", SCR_WIDTH, SCR_HEIGHT));
  GLFWwindow* windowHandle = window.GetHandle();

  Input::Init(&window);

  Chunk chunk({ 0, 0 });

  // texture
  Texture texture("dirt.png");
  texture.Use();

  Shader shader("main");

  Camera camera({ 0, 63, 16 });

  Input::SubscribeKeyCallback(GLFW_KEY_F3, [&](int action, int mods) {
    if (action == GLFW_PRESS) {
      showDebugInfo = !showDebugInfo;
    }
  });

  window.Setup();

  ImFont* font = window.GetImGuiIO().Fonts->AddFontFromFileTTF("res/fonts/Minecraft.ttf", 20.0f, NULL);

  int fps = 0;
  double lastFPSCheck = -1.0;
  double currTime = glfwGetTime();

  while (window.IsRunning()) {
    window.BeginFrame();

    currTime = glfwGetTime();
    if (currTime - lastFPSCheck > 0.5) {
      fps = Time::averageFPS;
      lastFPSCheck = currTime;
    }

    if (showDebugInfo) {
      ImGui::PushFont(font);
      ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize;
      bool open_ptr = true;
      ImGui::SetNextWindowPos({ 0, 0 });
      ImGui::Begin("Debug information", &open_ptr, windowFlags);
      ImGui::Text("FPS: %d", fps);
      ImGui::End();
      ImGui::PopFont();
    }

    camera.Update(windowHandle);

    glm::mat4 view = camera.CreateViewMatrix();
    projection = glm::perspective(glm::radians(FOV), window.GetAspectRatio(), 0.01f, 1000.0f);

    shader.Use();
    shader.LoadMatrix4f("model", glm::mat4(1.0f));
    shader.LoadMatrix4f("projection", projection);
    shader.LoadMatrix4f("view", view);

    chunk.Draw(shader);

    window.FinishFrame();
  }

  return 0;
}