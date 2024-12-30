#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <GLFW/glfw3.h>

#include "util/Logging.h"

class Camera {
public:
  Camera() = default;
  Camera(glm::vec3 position, float rotX = 0, float rotY = 0);

  void Update(GLFWwindow* window);
  glm::mat4 CreateViewMatrix() const;
  glm::vec3 GetForwardVector() const;
  glm::vec3 GetPosition() const;
  glm::vec2 GetRotation() const;

private:
  glm::vec3 m_position;
  float m_rotX, m_rotY;
  float m_speed = 1.0f;
};