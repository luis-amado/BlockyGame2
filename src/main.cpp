#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "util/Logging.h"
#include "util/OptionalMacros.h"
#include "engine/rendering/Shader.h"
#include "engine/rendering/textures/TextureAtlasBuilder.h"
#include "engine/rendering/textures/TextureAtlas.h"
#include "engine/rendering/Mesh.h"
#include "engine/camera/Camera.h"
#include "engine/io/Window.h"
#include "engine/io/Input.h"
#include "engine/io/Time.h"
#include "debug/DebugInformation.h"
#include "debug/DebugSettings.h"

#include "world/World.h"

#include "init/Blocks.h"

const int SCR_WIDTH = 800;
const int SCR_HEIGHT = 800;

bool showDebugInfo = false;

glm::mat4 projection;

int main() {
  // Logger::showFiles();
  Logger::setLogLevel(INFO);

  ASSIGN_OR_DIE(Window window, Window::CreateWindow("LuisCraft", SCR_WIDTH, SCR_HEIGHT));
  GLFWwindow* windowHandle = window.GetHandle();

  Input::Init(&window);

  Camera camera({ 0, 90, 16 });
  World world(camera);
  world.Start();

  Blocks::InitializeBlocks();
  Blocks::GenerateBlockAtlas();

  Shader shader("main");

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
      world.Update();
    }

    camera.Update(windowHandle);

    glm::mat4 view = camera.CreateViewMatrix();
    projection = glm::perspective(glm::radians(DebugSettings::instance.fov), window.GetAspectRatio(), 0.01f, 1000.0f);

    shader.Use();
    shader.LoadMatrix4f("projection", projection);
    shader.LoadMatrix4f("view", view);

    world.Draw(shader);

    DebugInformation::ShowIfActive(world, camera);

    window.FinishFrame();
  }

  return 0;
}