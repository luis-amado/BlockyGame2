#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "util/Logging.h"
#include "util/OptionalMacros.h"
#include "engine/rendering/Shader.h"
#include "engine/rendering/Texture.h"
#include "engine/rendering/Mesh.h"
#include "engine/camera/Camera.h"
#include "engine/io/Window.h"
#include "engine/io/Input.h"

#include "world/Chunk.h"

const int SCR_WIDTH = 800;
const int SCR_HEIGHT = 800;
const float FOV = 70.0f;

glm::mat4 projection;

int main() {
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

  // SETUP IMGUI
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  ImGui_ImplGlfw_InitForOpenGL(windowHandle, true);
  ImGui_ImplOpenGL3_Init("#version 330");

  while (window.IsRunning()) {
    window.BeginFrame();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("This is my weird window");
    ImGui::Text("Hello there");
    if (ImGui::Button("WOW")) {
      LOG(INFO) << "THATS COOL";
    }
    ImGui::End();

    camera.Update(windowHandle);

    glm::mat4 view = camera.CreateViewMatrix();
    projection = glm::perspective(glm::radians(FOV), window.GetAspectRatio(), 0.01f, 1000.0f);

    shader.Use();
    shader.LoadMatrix4f("model", glm::mat4(1.0f));
    shader.LoadMatrix4f("projection", projection);
    shader.LoadMatrix4f("view", view);

    chunk.Draw(shader);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    window.FinishFrame();
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  return 0;
}