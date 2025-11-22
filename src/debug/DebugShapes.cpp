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
    0.0f, 0.0f, 0.0f,  1.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 0.0f,  1.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 1.0f,  1.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 1.0f,  1.0f, 1.0f, 1.0f,
    0.0f, 1.0f, 0.0f,  1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 0.0f,  1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,  1.0f, 1.0f, 1.0f,
    0.0f, 1.0f, 1.0f,  1.0f, 1.0f, 1.0f,
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

}

void DebugShapes::DrawBoundingBox(BoundingBox boundingBox, glm::dvec3 position) {
  static LineMesh mesh = CreateBoxMesh();

  Shader& shader = ShaderLibrary::GetInstance().Get("line");

  glm::mat4 model(1.0f);
  model = glm::translate(model, static_cast<glm::vec3>(position));
  model = glm::scale(model, { boundingBox.width, boundingBox.height, boundingBox.width });
  model = glm::translate(model, { -0.5f, 0.0f, -0.5f });

  shader.Use();
  shader.LoadMatrix4f("model", model);

  mesh.Draw();
}