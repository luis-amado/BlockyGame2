#include "UITexturedQuad.h"
#include "../../ShaderLibrary.h"

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

UITexturedQuad::UITexturedQuad(Texture* texture, UIProps props)
  : UIQuad(props), m_texture(texture) {}

void UITexturedQuad::DrawMyself() const {
  UIQuadMesh& mesh = GetQuadMesh();
  glm::mat4 model = GetModelMatrix();

  Shader& shader = ShaderLibrary::GetInstance().Get("textured_ui");
  shader.Use();
  shader.LoadMatrix4f("model", model);

  m_texture->Use();

  mesh.Draw();
}

void UITexturedQuad::SetTexture(Texture* texture) {
  m_texture = texture;
}
