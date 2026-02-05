#include "Shader.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "util/FileUtil.h"
#include "util/Logging.h"

namespace {

enum ShaderType {
  VERTEX,
  FRAGMENT
};

unsigned int CompileShaderSource(const char* source, ShaderType shaderType) {
  unsigned int shader;
  GLuint glShaderType = shaderType == VERTEX ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER;
  shader = glCreateShader(glShaderType);
  glShaderSource(shader, 1, &source, NULL);
  glCompileShader(shader);

  int success;
  char infoLog[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    LOG(ERROR) << "SHADER COMPILATION ERROR:\n" << infoLog << '\n';
  }

  return shader;
}

} // namespace

Shader::Shader(const std::string& shaderName)
  : Shader(shaderName, shaderName) {}

Shader::Shader(const std::string& vertexShaderName, const std::string& fragmentShaderName) {
  std::string vertexSourceCode = FileUtil::ReadSource("res/shaders/" + vertexShaderName + ".vert");
  std::string fragmentSourceCode = FileUtil::ReadSource("res/shaders/" + fragmentShaderName + ".frag");

  const char* vertexSource = vertexSourceCode.c_str();
  const char* fragmentSource = fragmentSourceCode.c_str();

  unsigned int vertexShader = CompileShaderSource(vertexSource, VERTEX);
  unsigned int fragmentShader = CompileShaderSource(fragmentSource, FRAGMENT);

  // Link the shaders into a program
  m_shaderProgram = glCreateProgram();
  glAttachShader(m_shaderProgram, vertexShader);
  glAttachShader(m_shaderProgram, fragmentShader);
  glLinkProgram(m_shaderProgram);

  int success;
  char infoLog[512];
  glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(m_shaderProgram, 512, NULL, infoLog);
    LOG(ERROR) << "FAILED TO LINK SHADER:\n" << infoLog << '\n';
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
}

Shader::~Shader() {
  glDeleteProgram(m_shaderProgram);
}

void Shader::Use() const {
  if (m_shaderProgram != 0) {
    glUseProgram(m_shaderProgram);
  }
}

void Shader::LoadMatrix4f(const std::string& uniform, const glm::mat4& matrix) {
  unsigned int location = GetUniformLocation(uniform);
  glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::LoadVector4f(const std::string& uniform, const glm::vec4& vec) {
  unsigned int location = GetUniformLocation(uniform);
  glUniform4fv(location, 1, glm::value_ptr(vec));
}

void Shader::LoadVector3f(const std::string& uniform, const glm::vec3& vec) {
  unsigned int location = GetUniformLocation(uniform);
  glUniform3fv(location, 1, glm::value_ptr(vec));
}

void Shader::LoadVector2f(const std::string& uniform, const glm::vec2& vec) {
  unsigned int location = GetUniformLocation(uniform);
  glUniform2fv(location, 1, glm::value_ptr(vec));
}

void Shader::LoadInt(const std::string& uniform, int value) {
  unsigned int location = GetUniformLocation(uniform);
  glUniform1i(location, value);
}

void Shader::LoadBool(const std::string& uniform, bool value) {
  unsigned int location = GetUniformLocation(uniform);
  glUniform1i(location, value);
}

unsigned int Shader::GetUniformLocation(const std::string& uniform) {
  if (m_uniformMap.find(uniform) == m_uniformMap.end()) {
    unsigned int location = glGetUniformLocation(m_shaderProgram, uniform.c_str());
    m_uniformMap[uniform] = location;
  }
  return m_uniformMap[uniform];
}
