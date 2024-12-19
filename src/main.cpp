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

  Camera camera({ 0, 20, 0 });

  while (window.IsRunning()) {
    window.BeginFrame();

    camera.Update(windowHandle);

    glm::mat4 view = camera.CreateViewMatrix();
    projection = glm::perspective(glm::radians(FOV), window.GetAspectRatio(), 0.01f, 100.0f);

    shader.Use();
    shader.LoadMatrix4f("projection", projection);
    shader.LoadMatrix4f("view", view);

    chunk.Draw(shader);

    window.FinishFrame();
  }

  return 0;
}