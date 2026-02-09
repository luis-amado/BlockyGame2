#include "ShaderLibrary.h"
#include "util/Logging.h"

ShaderLibrary& ShaderLibrary::GetInstance() {
  static ShaderLibrary shaderLibrary;
  return shaderLibrary;
}

void ShaderLibrary::LoadShaders() {
  // This function must contain all the shaders used
  Load("main");
  Load("line");
  Load("colored_lines");
  Load("shape");
  Load("textured_ui");
  Load("colored_ui");
}

void ShaderLibrary::ReloadShaders() {
  m_shaders.clear();
  LoadShaders();
  m_reloadCallback();
}

void ShaderLibrary::OnReloadShaders(std::function<void()> callback) {
  m_reloadCallback = callback;
}

Shader& ShaderLibrary::Get(const std::string& name) const {
#ifdef DEBUG
  if (!m_shaders.count(name)) {
    LOG(FATAL) << "Shader not found: " << name;
}
#endif
  return *m_shaders.at(name);
}

void ShaderLibrary::Clear() {
  m_shaders.clear();
}

void ShaderLibrary::LoadMatrix4f(const std::string& uniform, glm::mat4 matrix) {
  for (auto& [_, shader] : m_shaders) {
    shader->Use();
    shader->LoadMatrix4f(uniform, matrix);
  }
}

void ShaderLibrary::LoadVector2f(const std::string& uniform, glm::vec2 vec) {
  for (auto& [_, shader] : m_shaders) {
    shader->Use();
    shader->LoadVector2f(uniform, vec);
  }
}

void ShaderLibrary::Load(const std::string& name) {
  m_shaders[name] = std::make_unique<Shader>(name);
}
