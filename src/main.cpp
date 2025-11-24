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
#include "engine/io/Window.h"
#include "engine/io/Input.h"
#include "engine/io/Time.h"
#include "debug/DebugInformation.h"
#include "debug/DebugSettings.h"
#include "entity/PlayerEntity.h"
#include "engine/rendering/ShaderLibrary.h"

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

  Input::Init(&window);

  PlayerEntity player;
  player.SetPosition({ 0.0, 90.0, 16.0 });

  World world(player);
  world.Start();

  Blocks::InitializeBlocks();
  Blocks::GenerateBlockAtlas();

  ShaderLibrary& shaders = ShaderLibrary::GetInstance();
  shaders.LoadShaders();

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

    player.Update();
    player.PhysicsUpdate(world);

    glm::mat4 view = player.GetFirstPersonViewMatrix();
    projection = glm::perspective(glm::radians(DebugSettings::instance.fov), window.GetAspectRatio(), 0.01f, 1000.0f);

    shaders.LoadMatrix4f("projection", projection);
    shaders.LoadMatrix4f("view", view);

    world.Draw();

    DebugInformation::ShowIfActive(world, player);

    window.FinishFrame();
  }

  shaders.Clear();

  return 0;
}