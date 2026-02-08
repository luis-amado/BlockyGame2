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

void ShaderLibrary::Load(const std::string& name) {
  m_shaders[name] = std::make_unique<Shader>(name);
}
