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
  void LoadVector3f(const std::string& uniform, const glm::vec3& vec);
  void LoadBool(const std::string& uniform, bool value);

private:
  unsigned int m_shaderProgram;
  std::unordered_map<std::string, unsigned int> m_uniformMap;

  unsigned int GetUniformLocation(const std::string& uniform);
};