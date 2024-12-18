#pragma once

#include <string>
#include <glm/mat4x4.hpp>
#include <unordered_map>

#include "util/ClassMacros.h"

class Shader {

public:
  explicit Shader(const std::string& shaderName);
  Shader(const std::string& vertexShaderName, const std::string& fragmentShaderName);
  ~Shader();

  ONLY_MOVE_ID(Shader, m_shaderProgram);

  void Use() const;
  void LoadMatrix4f(const std::string& uniform, const glm::mat4& matrix);

private:
  unsigned int m_shaderProgram;
  std::unordered_map<std::string, unsigned int> m_uniformMap;

  unsigned int GetUniformLocaction(const std::string& uniform);
};