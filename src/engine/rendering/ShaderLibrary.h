#pragma once

#include "Shader.h"
#include <glm/mat4x4.hpp>
#include <functional>

class ShaderLibrary {
public:
  static ShaderLibrary& GetInstance();

  void LoadShaders();
  void ReloadShaders();
  void OnReloadShaders(std::function<void()> callback);

  Shader& Get(const std::string& name) const;
  void Clear();

  void LoadMatrix4f(const std::string& uniform, glm::mat4 matrix);
  void LoadVector2f(const std::string& uniform, glm::vec2 vec);

private:
  ShaderLibrary() = default;
  ~ShaderLibrary() = default;

  void Load(const std::string& name);

  std::function<void()> m_reloadCallback;
  std::unordered_map<std::string, std::unique_ptr<Shader>> m_shaders;
};
