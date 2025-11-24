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
  return { 0.75, 1.7 };
}

double PlayerEntity::GetEyeLevel() const {
  return 1.55;
}

void PlayerEntity::Update() {
  // Update position relative to its forward direction
  glm::dvec3 forward = GetForwardVector();
  glm::dvec3 right = GetRightVector(forward);

  glm::dvec3 newVelocity = { 0.0, 0.0, 0.0 };

  if (Input::IsKeyPressed(GLFW_KEY_A)) {
    newVelocity -= right * m_walkSpeed;
  }
  if (Input::IsKeyPressed(GLFW_KEY_D)) {
    newVelocity += right * m_walkSpeed;
  }
  if (Input::IsKeyPressed(GLFW_KEY_W)) {
    newVelocity += forward * m_walkSpeed;
  }
  if (Input::IsKeyPressed(GLFW_KEY_S)) {
    newVelocity -= forward * m_walkSpeed;
  }
  if (Input::IsKeyPressed(GLFW_KEY_SPACE)) {
    Jump();
  }

  m_velocity.x = newVelocity.x;
  m_velocity.z = newVelocity.z;

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
