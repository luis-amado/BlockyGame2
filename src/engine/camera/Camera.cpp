#include <glad/glad.h>
#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include "../io/Input.h"
#include "../io/Time.h"

namespace {
double sensitivity = 0.1;
} // namespace

Camera::Camera(glm::vec3 position, float rotX, float rotY)
  : m_position(position), m_rotX(rotX), m_rotY(rotY) {}

void Camera::Update(GLFWwindow* window) {

  double deltaTime = Time::deltaTime;

  // Update camera position relative to its forward direction
  glm::vec3 forward = GetForwardVector();
  glm::vec3 right = glm::cross(forward, { 0, 1, 0 });

  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    m_position -= right * (float)(m_speed * deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    m_position += right * (float)(m_speed * deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    m_position += forward * (float)(m_speed * deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    m_position -= forward * (float)(m_speed * deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
    m_position.y -= m_speed * deltaTime;
  }
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
    m_position.y += m_speed * deltaTime;
  }

  // Camera rotation
  m_rotY += Input::GetMouseDX() * sensitivity;
  m_rotX += Input::GetMouseDY() * sensitivity;

  // Normalize camera rotation
  m_rotY = std::fmodf(m_rotY, 360.0f);

  // Lock max vertical rotation to straight up and straight down
  if (m_rotX > 90.0f) {
    m_rotX = 90.0f;
  } else if (m_rotX < -90.0f) {
    m_rotX = -90.0f;
  }

}

glm::mat4 Camera::CreateViewMatrix() const {

  glm::mat4 view(1.0f);

  // Rotate
  view = glm::rotate(view, glm::radians(m_rotX), { 1, 0, 0 });
  view = glm::rotate(view, glm::radians(m_rotY), { 0, 1, 0 });
  // Translate
  view = glm::translate(view, -m_position);

  return view;
}

glm::vec3 Camera::GetForwardVector() const {
  return {
    std::sinf(glm::radians(m_rotY)),
    0,
    -std::cosf(glm::radians(m_rotY))
  };
}
