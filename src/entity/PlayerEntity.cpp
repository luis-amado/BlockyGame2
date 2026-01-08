#include <glad/glad.h>
#include "PlayerEntity.h"

#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include "../engine/io/Time.h"
#include "../engine/io/Input.h"
#include "util/MathUtil.h"
#include "../block/Block.h"
#include "../debug/DebugShapes.h"
#include "util/Logging.h"
#include "../init/Blocks.h"
#include "../physics/AABB.h"

namespace {
const double sensitivity = 0.1;

// double RayDistanceToPlane(const glm::dvec3& origin, const glm::dvec3& direction, const glm::dvec3& normal, const glm::dvec3& center) {
//   double denom = glm::dot(direction, normal);
//   if (abs(denom) > 0.0001) {
//     double t = glm::dot((center - origin), normal) / denom;
//     if (t > 0.0) return t;
//   }
//   return std::numeric_limits<double>::infinity();
// }

// TODO: Fix and use this method instead
// std::pair<double, glm::ivec3> RaycastTravelNextBlock(const glm::dvec3& origin, const glm::dvec3& direction, const glm::ivec3& position) {
//   glm::dvec3 lowerCorner = position;
//   glm::dvec3 upperCorner = glm::dvec3(position) + glm::dvec3 { 1.0, 1.0, 1.0 };

//   std::vector<std::pair<double, glm::ivec3>> distances = {
//     { RayDistanceToPlane(origin, direction, { 1, 0, 0 }, lowerCorner), {-1,  0,  0} },
//     { RayDistanceToPlane(origin, direction, { 1, 0, 0 }, upperCorner), { 1,  0,  0} },
//     { RayDistanceToPlane(origin, direction, { 0, 1, 0 }, lowerCorner), { 0, -1,  0} },
//     { RayDistanceToPlane(origin, direction, { 0, 1, 0 }, upperCorner), { 0,  1,  0} },
//     { RayDistanceToPlane(origin, direction, { 0, 0, 1 }, lowerCorner), { 0,  0, -1} },
//     { RayDistanceToPlane(origin, direction, { 0, 0, 1 }, upperCorner), { 0,  0,  1} }
//   };

//   double minDistance = std::numeric_limits<double>::max();
//   glm::ivec3 offset;

//   for (int i = 0; i < 6; i++) {
//     if (distances[i].first < minDistance) {
//       minDistance = distances[i].first;
//       offset = distances[i].second;
//     }
//   }

//   return { minDistance, offset };
// }

// std::optional<glm::ivec3> RaycastToBlockHit(const World& world, glm::dvec3 origin, const glm::dvec3& direction, double range) {
//   double distance = 0.0;

//   origin += direction * 0.001;

//   // start at the block of origin
//   // check distances to the 6 adjacent planes to the block
//   // travel along the axis of the least distance and repeat

//   // current block position
//   glm::ivec3 bPos = glm::floor(origin);
//   int maxSteps = 20;
//   while (distance <= range && maxSteps > 0) {
//     if (!Block::FromBlockstate(world.GetBlockstateAt(bPos.x, bPos.y, bPos.z)).IsAir()) {
//       return bPos;
//     }
//     auto [newDistance, offset] = RaycastTravelNextBlock(origin + ((distance + 0.001) * direction), direction, bPos);
//     distance += newDistance;
//     bPos += offset;

//     maxSteps--;
//   }

//   return std::nullopt;
// }

std::optional<std::pair<glm::ivec3, glm::ivec3>> RaycastToBlockHit2(const World& world, glm::dvec3 origin, const glm::dvec3& direction, double range) {
  double step = 0.01;
  glm::ivec3 prevPos = origin;

  for (double distance = 0.0; distance <= range; distance += step) {
    glm::ivec3 pos = glm::floor(origin + direction * distance);
    if (!Block::FromBlockstate(world.GetBlockstateAt(XYZ(pos))).IsAir()) {
      return std::pair<glm::ivec3, glm::ivec3> { pos, prevPos };
    }
    prevPos = pos;
  }

  return std::nullopt;
}

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

double PlayerEntity::GetSpeedMultiplier() const {
  return m_speedMultiplier;
}

void PlayerEntity::Update(World& world) {

  // Update rotation
  if (!Input::IsCursorShown()) {

    glm::dvec2 rot = GetRotation();

    if (m_zoomed) {
      m_rotationVelocity.y += Input::GetMouseDX() * sensitivity * 1.2;
      m_rotationVelocity.x += Input::GetMouseDY() * sensitivity * 1.2;

      m_rotationVelocity.y = MathUtil::Lerp(m_rotationVelocity.y, 0.0, Time::deltaTime / 0.8);
      m_rotationVelocity.x = MathUtil::Lerp(m_rotationVelocity.x, 0.0, Time::deltaTime / 0.8);

      rot.y += m_rotationVelocity.y * Time::deltaTime;
      rot.x += m_rotationVelocity.x * Time::deltaTime;
    } else {
      m_rotationVelocity.x = 0.0;
      m_rotationVelocity.y = 0.0;
      rot.y += Input::GetMouseDX() * sensitivity;
      rot.x += Input::GetMouseDY() * sensitivity;
    }

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

  // Update position relative to its forward direction
  glm::dvec3 forward = GetForwardVector();
  glm::dvec3 right = GetRightVector(forward);
  glm::dvec3 newVelocity = { 0.0, 0.0, 0.0 };

  // Check for flying
  if (Input::IsJustDoublePressed(KEY_SPACE)) {
    m_flying = !m_flying;
    SetDisableGravity(m_flying);
    if (!m_flying) {
      m_ghost = false;
      m_speedMultiplier = 1.0;
      SetDisableCollision(false);
    }
  }

  // Zooming
  m_zoomed = Input::IsPressed('C');

  // Check for sprinting
  if (Input::IsJustDoublePressed('W')) {
    m_sprinting = true;
  } else if (m_sprinting && !Input::IsPressed('W')) {
    m_sprinting = false;
  }

  // Check for ghost
  if (Input::IsJustPressed('G')) {
    m_ghost = !m_ghost;
    if (m_ghost) {
      m_flying = true;
      SetDisableGravity(true);
    }
    SetDisableCollision(m_ghost);
  }

  m_speedMultiplier += Input::GetMouseDWheel() * (1.0 / 3.0 * 0.1);
  m_speedMultiplier = MathUtil::Clamp(m_speedMultiplier, 0.1, 300.0);

  double speed = m_walkSpeed * m_speedMultiplier;

  if (Input::IsPressed('A')) {
    newVelocity -= right * speed;
  }
  if (Input::IsPressed('D')) {
    newVelocity += right * speed;
  }
  if (Input::IsPressed('W')) {
    newVelocity += forward * speed * (m_sprinting ? m_sprintMultiplier : 1.0);
  }
  if (Input::IsPressed('S')) {
    newVelocity -= forward * speed;
  }

  if (m_flying) {
    if (Input::IsPressed(KEY_SPACE)) {
      newVelocity.y += speed;
    }
    if (Input::IsPressed(KEY_LEFT_SHIFT)) {
      newVelocity.y -= speed;
    }
  } else {
    if (Input::IsPressed(KEY_SPACE)) {
      if (IsGrounded()) {
        if (m_timeSinceLastAutoJump > 0.02) {
          Jump();
        } else {
          m_timeSinceLastAutoJump += Time::deltaTime;
        }
      } else {
        m_timeSinceLastAutoJump = 0.0;
      }
    } else {
      m_timeSinceLastAutoJump = std::numeric_limits<double>::max();
    }
  }

  if (m_flying) {
    m_velocity.y = newVelocity.y;
  }
  m_velocity.x = newVelocity.x;
  m_velocity.z = newVelocity.z;

  PhysicsUpdate(world);

  // Player interactions with the world
  // Physics calculations have been settled at this point

  UpdateLookingAt(world);

  if (!Input::IsCursorShown() && Input::IsJustPressed(MOUSE_BTN_LEFT) && m_lookingAtBlock.has_value()) {
    world.UpdateBlockstateAt(m_lookingAtBlock->x, m_lookingAtBlock->y, m_lookingAtBlock->z, Blocks::AIR.GetBlockstate());
  }
  if (!Input::IsCursorShown() && Input::IsJustPressed(MOUSE_BTN_RIGHT) && m_placingAtBlock.has_value()) {
    char blockToPlace = Blocks::GLOWSTONE.GetBlockstate();
    if (m_ghost) {
      world.UpdateBlockstateAt(m_placingAtBlock->x, m_placingAtBlock->y, m_placingAtBlock->z, blockToPlace);
    } else {
      BoundingBox bb = GetBoundingBox();
      AABB playerAABB = AABB::CreateFromBottomCenter(GetPosition(), bb.width, bb.height);
      AABB blockAABB = AABB::CreateFromMinCorner(m_placingAtBlock.value(), 1.0, 1.0);

      if (!playerAABB.IsColliding(blockAABB)) {
        world.UpdateBlockstateAt(m_placingAtBlock->x, m_placingAtBlock->y, m_placingAtBlock->z, blockToPlace);
      }
    }
  }
}

float PlayerEntity::GetFOVChange() const {
  if (m_sprinting) {
    return 1.2f;
  } else {
    return 1.0f;
  }
}

std::optional<float> PlayerEntity::GetFOVOverride() const {
  if (m_zoomed) {
    return 20.0f;
  } else {
    return std::nullopt;
  }
}

bool PlayerEntity::CanFly() const {
  return m_flying;
}


void PlayerEntity::UpdateLookingAt(const World& world) {
  glm::dvec3 eyesPosition = GetPosition() + glm::dvec3 { 0.0, GetEyeLevel(), 0.0 };
  glm::dvec3 direction = glm::normalize(GetLookingVector());

  // TODO: Fix RaycastToBlockHit1 (sometimes chooses the wrong position)
  std::optional<std::pair<glm::ivec3, glm::ivec3>> result = RaycastToBlockHit2(world, eyesPosition, direction, 7.0);
  if (result.has_value()) {
    m_lookingAtBlock = result->first;
    m_placingAtBlock = result->second;
  } else {
    m_lookingAtBlock = std::nullopt;
    m_placingAtBlock = std::nullopt;
  }
}

const std::optional<glm::ivec3>& PlayerEntity::GetLookingAtBlock() const {
  return m_lookingAtBlock;
}
