#include "UIColoredQuad.h"
#include "../../ShaderLibrary.h"

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

UIColoredQuad::UIColoredQuad(Color color, UIProps props)
  : UIQuad(props), m_color(color) {}

void UIColoredQuad::DrawMyself() const {
  UIQuadMesh& mesh = GetQuadMesh();
  glm::mat4 model = GetModelMatrix();

  Shader& shader = ShaderLibrary::GetInstance().Get("colored_ui");
  shader.Use();
  shader.LoadMatrix4f("model", model);
  shader.LoadVector4f("color", m_color.GetRGBA());

  mesh.Draw();
}

void UIColoredQuad::SetColor(Color color) {
  m_color = color;
}
