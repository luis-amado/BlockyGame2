#include <glad/glad.h>
#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include "../io/Input.h"
#include "../io/Time.h"
#include "util/MathUtil.h"

namespace {
double sensitivity = 0.1;
} // namespace

Camera::Camera(glm::vec3 position, float rotX, float rotY)
  : m_position(position), m_rotX(rotX), m_rotY(rotY) {}

void Camera::Update(GLFWwindow* window) {

  // Use scroll wheel to update speed
  m_speed += Input::GetMouseDWheel() * 0.1f;
  m_speed = std::max(0.0f, m_speed);

  double deltaTime = Time::deltaTime;

  // Update camera position relative to its forward direction
  glm::vec3 forward = GetForwardVector();
  glm::vec3 right = glm::cross(forward, { 0, 1, 0 });

  glm::vec3 newPosition = m_position.Get();

  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    newPosition -= right * (float)(m_speed * deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    newPosition += right * (float)(m_speed * deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    newPosition += forward * (float)(m_speed * deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    newPosition -= forward * (float)(m_speed * deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
    newPosition.y -= m_speed * deltaTime;
  }
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
    newPosition.y += m_speed * deltaTime;
  }

  m_position.Set(newPosition);

  // Camera rotation
  if (!Input::IsCursorShown()) {
    m_rotY += Input::GetMouseDX() * sensitivity;
    m_rotX += Input::GetMouseDY() * sensitivity;

    // Normalize camera rotation
    m_rotY = MathUtil::fMod(m_rotY, 360.0f);

    // Lock max vertical rotation to straight up and straight down
    if (m_rotX > 90.0f) {
      m_rotX = 90.0f;
    } else if (m_rotX < -90.0f) {
      m_rotX = -90.0f;
    }
  }
}

glm::mat4 Camera::CreateViewMatrix() const {

  glm::mat4 view(1.0f);

  // Rotate
  view = glm::rotate(view, glm::radians(m_rotX), { 1, 0, 0 });
  view = glm::rotate(view, glm::radians(m_rotY), { 0, 1, 0 });
  // Translate
  view = glm::translate(view, -m_position.Get());

  return view;
}

glm::vec3 Camera::GetForwardVector() const {
  return {
    std::sinf(glm::radians(m_rotY)),
    0,
    -std::cosf(glm::radians(m_rotY))
  };
}

glm::vec3 Camera::GetPosition() const {
  return m_position.Get();
}

glm::vec2 Camera::GetRotation() const {
  return { m_rotX, m_rotY };
}

float Camera::GetSpeed() const {
  return m_speed;
}
