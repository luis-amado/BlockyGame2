#include <glad/glad.h>
#include "PlayerEntity.h"

#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include "../engine/io/Time.h"
#include "../engine/io/Input.h"

namespace {
const double sensitivity = 0.1;
}  // namespace

PlayerEntity::PlayerEntity() {}

glm::mat4 PlayerEntity::GetFirstPersonViewMatrix() {
  glm::mat4 view(1.0f);

  const glm::dvec2& rot = GetRotation();

  // Rotate
  view = glm::rotate(view, glm::radians((float)rot.x), { 1, 0, 0 });
  view = glm::rotate(view, glm::radians((float)rot.y), { 0, 1, 0 });
  // Translate
  glm::dvec3 eyePosition = GetPosition() + glm::dvec3(0.0, GetEyeLevel(), 0.0);
  view = glm::translate(view, -static_cast<glm::vec3>(eyePosition));

  return view;
}

BoundingBox PlayerEntity::GetBoundingBox() const {
  return { 0.8, 1.7 };
}

double PlayerEntity::GetEyeLevel() const {
  return 1.55;
}

void PlayerEntity::Update() {
  double deltaTime = Time::deltaTime;

  // Update position relative to its forward direction
  glm::dvec3 forward = GetForwardVector();
  glm::dvec3 right = GetRightVector(forward);

  glm::dvec3 newPosition = GetPosition();

  if (Input::IsKeyPressed(GLFW_KEY_A)) {
    newPosition -= right * (m_walkSpeed * deltaTime);
  }
  if (Input::IsKeyPressed(GLFW_KEY_D)) {
    newPosition += right * (m_walkSpeed * deltaTime);
  }
  if (Input::IsKeyPressed(GLFW_KEY_W)) {
    newPosition += forward * (m_walkSpeed * deltaTime);
  }
  if (Input::IsKeyPressed(GLFW_KEY_S)) {
    newPosition -= forward * (m_walkSpeed * deltaTime);
  }
  if (Input::IsKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
    newPosition.y -= m_walkSpeed * deltaTime;
  }
  if (Input::IsKeyPressed(GLFW_KEY_SPACE)) {
    newPosition.y += m_walkSpeed * deltaTime;
  }

  SetPosition(newPosition);

  // Update rotation
  if (!Input::IsCursorShown()) {

    glm::dvec2 rot = GetRotation();

    rot.y += Input::GetMouseDX() * sensitivity;
    rot.x += Input::GetMouseDY() * sensitivity;

    // Normalize camera rotation
    rot.y = std::fmod(rot.y, 360.0);

    // Lock max vertical rotation to straight up and straight down
    if (rot.x > 90.0f) {
      rot.x = 90.0f;
    } else if (rot.x < -90.0f) {
      rot.x = -90.0f;
    }

    SetRotation(rot);
  }
}
