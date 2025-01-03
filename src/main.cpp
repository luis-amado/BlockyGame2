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
#include "engine/io/Time.h"
#include "debug/DebugInformation.h"
#include "debug/DebugSettings.h"

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
  world.Start();

  // texture
  Texture texture = Texture("stone_bricks.png");
  texture.Use();

  Shader shader("main");

  Camera camera({ 0, 83, 16 });

  Input::SubscribeKeyCallback(GLFW_KEY_F3, [](int action, int mods) {
    if (action == GLFW_PRESS) {
      DebugInformation::Toggle();
    }
  });

  window.Setup();
  DebugInformation::Setup(window);

  while (window.IsRunning()) {
    window.BeginFrame();

    if (DebugSettings::instance.updateWorld) {
      world.Update(camera.GetPosition());
    }

    camera.Update(windowHandle);

    glm::mat4 view = camera.CreateViewMatrix();
    projection = glm::perspective(glm::radians(FOV), window.GetAspectRatio(), 0.01f, 1000.0f);

    shader.Use();
    shader.LoadMatrix4f("projection", projection);
    shader.LoadMatrix4f("view", view);

    world.Draw(shader);

    DebugInformation::ShowIfActive(world, camera);

    window.FinishFrame();
  }

  return 0;
}