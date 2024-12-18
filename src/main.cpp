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
#include "engine/camera/Camera.h"
#include "engine/io/Window.h"
#include "engine/io/Input.h"

const int SCR_WIDTH = 800;
const int SCR_HEIGHT = 800;
const float FOV = 70.0f;

bool cursorShown = false;
bool fullscreen = false;
int win_xpos = 0, win_ypos = 0, win_width = SCR_WIDTH, win_height = SCR_HEIGHT;

glm::mat4 projection;

int main() {
  Logger::setLogLevel(INFO);

  ASSIGN_OR_DIE(Window window, Window::CreateWindow("LuisCraft", SCR_WIDTH, SCR_HEIGHT));
  GLFWwindow* windowHandle = window.GetHandle();

  Input::Init(&window);

  // vertices
  float vertices[] = {
    // NORTH (-z)
    1.0f, 1.0f, 0.0f,  0.0f, 1.0f,
    1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
    0.0f, 0.0f, 0.0f,  1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
    // SOUTH (+z)
    0.0f, 1.0f, 1.0f,  0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
    1.0f, 0.0f, 1.0f,  1.0f, 0.0f,
    1.0f, 1.0f, 1.0f,  1.0f, 1.0f,
    // EAST (+x)
    1.0f, 1.0f, 1.0f,  0.0f, 1.0f,
    1.0f, 0.0f, 1.0f,  0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,  1.0f, 1.0f,
    // WEST (-x)
    0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
    0.0f, 0.0f, 0.0f,  0.0f, 0.0f,
    0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
    0.0f, 1.0f, 1.0f,  1.0f, 1.0f,
    // UP (+y)
    0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
    0.0f, 1.0f, 1.0f,  0.0f, 0.0f,
    1.0f, 1.0f, 1.0f,  1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,  1.0f, 1.0f,
    // DOWN (-y)
    1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
    1.0f, 0.0f, 1.0f,  0.0f, 0.0f,
    0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
    0.0f, 0.0f, 0.0f,  1.0f, 1.0f,
  };

  // indices
  unsigned int indices[] = {
     0,  1,  2,  2,  3,  0,
     4,  5,  6,  6,  7,  4,
     8,  9, 10, 10, 11,  8,
    12, 13, 14, 14, 15, 12,
    16, 17, 18, 18, 19, 16,
    20, 21, 22, 22, 23, 20,
  };

  // vao
  unsigned int VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  // vbo
  unsigned int VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  // ebo
  unsigned int EBO;
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  // texture
  Texture texture("dirt.png");
  texture.Use();

  // transformations
  projection = glm::perspective(glm::radians(FOV), 1.0f, 0.1f, 100.0f);

  glm::mat4 model(1.0f);
  model = glm::translate(model, glm::vec3(-0.5f, -0.5f, -0.5f));

  Shader shader("main");
  shader.Use();

  shader.LoadMatrix4f("model", model);

  Camera camera({ 0, 0, 4 });

  while (window.IsRunning()) {
    window.BeginFrame();

    camera.Update(windowHandle);

    glm::mat4 view = camera.CreateViewMatrix();
    projection = glm::perspective(glm::radians(FOV), window.GetAspectRatio(), 0.01f, 100.0f);

    shader.LoadMatrix4f("projection", projection);
    shader.LoadMatrix4f("view", view);

    glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, NULL);

    window.FinishFrame();
  }

  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  glDeleteVertexArrays(1, &VAO);

  return 0;
}