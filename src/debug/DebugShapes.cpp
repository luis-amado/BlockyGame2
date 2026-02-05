#include <glad/glad.h>
#include "DebugShapes.h"

#include "../engine/rendering/Shader.h"
#include "../engine/rendering/ShaderLibrary.h"
#include <glm/gtc/matrix_transform.hpp>
#include "util/Logging.h"

namespace {

LineMesh CreateBoxMesh() {
  LineMesh mesh;

  float vertices[] = {
    // pos             // color
    0.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 1.0f,
    0.0f, 1.0f, 1.0f,
  };

  unsigned int indices[] = {
    0, 1,
    1, 2,
    2, 3,
    3, 0,
    4, 5,
    5, 6,
    6, 7,
    7, 4,
    0, 4,
    1, 5,
    2, 6,
    3, 7,
  };

  mesh.SetData(vertices, 48, indices, 24);
  return mesh;
}

const LineMesh& GetBoxMesh() {
  static LineMesh mesh = CreateBoxMesh();
  return mesh;
}

const LineMesh CreateLineMesh() {
  LineMesh mesh;

  float vertices[] = {
    0.0f, 0.0f, 0.0f,
    1.0f, 1.0f, 1.0f,
  };
  unsigned int indices[] = {
    0, 1,
  };

  mesh.SetData(vertices, 6, indices, 2);
  return mesh;
}

const LineMesh& GetLineMesh() {
  static LineMesh mesh = CreateLineMesh();
  return mesh;
}

}

void DebugShapes::DrawBoundingBox(BoundingBox boundingBox, glm::dvec3 position) {
  const LineMesh& mesh = GetBoxMesh();
  Shader& shader = ShaderLibrary::GetInstance().Get("shape");

  glm::mat4 model(1.0f);
  model = glm::translate(model, static_cast<glm::vec3>(position));
  model = glm::scale(model, { boundingBox.width, boundingBox.height, boundingBox.width });
  model = glm::translate(model, { -0.5f, 0.0f, -0.5f });

  shader.Use();
  shader.LoadMatrix4f("model", model);
  shader.LoadVector3f("color", { 1.0f, 1.0f, 1.0f });

  mesh.Draw();
}

void DebugShapes::DrawBlockBox(glm::ivec3 position, glm::vec3 color) {
  const LineMesh& mesh = GetBoxMesh();
  Shader& shader = ShaderLibrary::GetInstance().Get("shape");

  glm::mat4 model(1.0f);
  model = glm::translate(model, static_cast<glm::vec3>(position));
  model = glm::translate(model, { 0.5f, 0.5f, 0.5f });
  model = glm::scale(model, { 1.005f, 1.005f, 1.005f });
  model = glm::translate(model, { -0.5f, -0.5f, -0.5f });

  shader.Use();
  shader.LoadMatrix4f("model", model);
  shader.LoadVector3f("color", color);

  mesh.Draw();
}

void DebugShapes::DrawLine(glm::dvec3 start, glm::dvec3 end, glm::vec3 color) {
  const LineMesh& mesh = GetLineMesh();
  Shader& shader = ShaderLibrary::GetInstance().Get("line");

  shader.Use();
  shader.LoadVector3f("start", start);
  shader.LoadVector3f("end", end);
  shader.LoadVector3f("color", color);

  mesh.Draw();
}

