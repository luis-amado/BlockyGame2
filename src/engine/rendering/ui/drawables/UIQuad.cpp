#include "UIQuad.h"
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../../ShaderLibrary.h"

namespace {

UIQuadMesh GenerateQuadMesh() {
  float vertices[] = {
    0.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 0.0f
  };
  unsigned int indices[] = {
    0, 1, 2, 2, 3, 0
  };
  UIQuadMesh quadMesh;
  quadMesh.SetData(vertices, sizeof(vertices) / sizeof(float), indices, sizeof(indices) / sizeof(unsigned int));
  return quadMesh;
}

}  // namespace

UIQuad::UIQuad(UIProps props) : UIBlock(props) {}

void UIQuad::DrawMyself() const {
  UIQuadMesh& mesh = GetQuadMesh();
  glm::mat4 model = GetModelMatrix();

  Shader& shader = ShaderLibrary::GetInstance().Get("ui");
  shader.Use();
  shader.LoadMatrix4f("model", model);

  mesh.Draw();
}

UIQuadMesh& UIQuad::GetQuadMesh() const {
  static UIQuadMesh quadMesh = GenerateQuadMesh();
  return quadMesh;
}

glm::mat4 UIQuad::GetModelMatrix() const {
  glm::mat4 model(1.0f);
  glm::vec2 position = GetPosition();
  model = glm::translate(model, { position.x, position.y, 0.0f });
  glm::vec2 size = GetSize();
  model = glm::scale(model, { size.x, size.y, 1.0f });

  return model;
}
