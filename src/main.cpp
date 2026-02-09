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
#include "engine/io/Window.h"
#include "engine/io/Input.h"
#include "engine/io/Time.h"
#include "debug/DebugInformation.h"
#include "debug/DebugSettings.h"
#include "debug/DebugShapes.h"
#include "entity/PlayerEntity.h"
#include "engine/rendering/ShaderLibrary.h"
#include "engine/rendering/buffers/ResourceGraveyard.h"
#include "world/World.h"
#include "init/Blocks.h"
#include "ui/GameUI.h"

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
  player.Setup(&world);

  Blocks::InitializeBlocks();
  Blocks::GenerateBlockAtlas();

  ShaderLibrary& shaders = ShaderLibrary::GetInstance();
  shaders.LoadShaders();

  window.Setup();
  DebugInformation::Setup(window);

  float fov = DebugSettings::instance.defaultFOV;
  float fovChangeTime = 0.1f;

  GameUI gameUI(player);
  gameUI.Update(window);
  window.OnResize([&](int width, int height) {
    gameUI.Update(window);
  });
  shaders.OnReloadShaders([&]() {
    gameUI.Update(window);
  });

  while (window.IsRunning()) {
    window.BeginFrame();

    if (DebugSettings::instance.updateWorld) {
      world.Update();
    }

    player.Update();

    glm::mat4 view = player.GetFirstPersonViewMatrix();

    fov = fov + (DebugSettings::instance.defaultFOV * player.GetFOVChange() - fov) * (Time::deltaTime / fovChangeTime);
    float currentFOV = player.GetFOVOverride().value_or(fov);

    projection = glm::perspective(glm::radians(currentFOV), window.GetAspectRatio(), 0.01f, 1000.0f);

    shaders.LoadMatrix4f("projection", projection);
    shaders.LoadMatrix4f("view", view);

    world.Draw();

    if (player.GetLookingAtBlock().has_value()) {
      DebugShapes::DrawBlockBox(*player.GetLookingAtBlock(), { 0.0f, 0.0f, 0.0f });
    }

    DebugInformation::ShowIfActive(world, player);

    gameUI.Draw();

    window.FinishFrame();
  }

  shaders.Clear();
  ResourceGraveyard::GetInstance().Flush();

  return 0;
}