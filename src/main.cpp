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

#include "world/World.h"

const int SCR_WIDTH = 800;
const int SCR_HEIGHT = 800;
const float FOV = 90.0f;

bool showDebugInfo = false;

glm::mat4 projection;

int main() {
  // Logger::showFiles();
  Logger::setLogLevel(INFO);

  ASSIGN_OR_DIE(Window window, Window::CreateWindow("LuisCraft", SCR_WIDTH, SCR_HEIGHT));
  GLFWwindow* windowHandle = window.GetHandle();

  Input::Init(&window);

  World world;

  // texture
  Texture texture("dirt.png");
  texture.Use();

  Shader shader("main");

  Camera camera({ 0, 83, 16 });

  Input::SubscribeKeyCallback(GLFW_KEY_F3, [](int action, int mods) {
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
      ImGui::SetNextWindowSize({ 1000, 1000 });
      ImGui::Begin("Debug information", &open_ptr, windowFlags);
      ImGui::Text("FPS: %d", fps);
      ImGui::Text("XYZ: %.5f / %.5f / %.5f", camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
      ImGui::Text("");

      glm::ivec2 chunkCoord = world.GetChunkCoord(floor(camera.GetPosition().x), floor(camera.GetPosition().z));
      glm::ivec3 localCoords = world.ToLocalCoords(floor(camera.GetPosition().x), camera.GetPosition().y, floor(camera.GetPosition().z));

      ImGui::Text("Chunk: %d, %d", chunkCoord.x, chunkCoord.y);
      ImGui::Text("Local XYZ: %d / %d / %d", localCoords.x, localCoords.y, localCoords.z);

      ImGui::Text("");

      if (world.GetBlockstateAt(floor(camera.GetPosition().x), camera.GetPosition().y, floor(camera.GetPosition().z))) {
        ImGui::Text("Inside Block");
      }

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

    world.Draw(shader);

    window.FinishFrame();
  }

  return 0;
}