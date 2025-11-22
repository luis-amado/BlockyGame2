#pragma once

#include "Shader.h"
#include <glm/mat4x4.hpp>

class ShaderLibrary {
public:
  static ShaderLibrary& GetInstance();

  void LoadShaders();
  Shader& Get(const std::string& name) const;
  void Clear();

  void LoadMatrix4f(const std::string& uniform, glm::mat4 matrix);

private:
  ShaderLibrary() = default;
  ~ShaderLibrary() = default;

  void Load(const std::string& name);

  std::unordered_map<std::string, std::unique_ptr<Shader>> m_shaders;
};
