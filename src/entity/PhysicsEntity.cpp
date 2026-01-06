#include "PhysicsEntity.h"
#include "../engine/io/Time.h"
#include "../world/World.h"
#include "../physics/AABB.h"
#include "../debug/DebugShapes.h"
#include "../block/Block.h"
#include "../debug/DebugSettings.h"

#include <limits>

PhysicsEntity::PhysicsEntity(glm::dvec3 pos, glm::dvec2 rot)
  : Entity(pos, rot) {}

double PhysicsEntity::GetGravity() const {
  return DebugSettings::instance.gravity;
}

double PhysicsEntity::GetJumpForce() const {
  return DebugSettings::instance.jumpForce;
}

const glm::dvec3& PhysicsEntity::GetVelocity() const {
  return m_velocity;
}

bool PhysicsEntity::IsGrounded() const {
  return m_grounded;
}

void PhysicsEntity::SetDisablePhysics(bool disablePhysics) {
  m_disablePhysics = disablePhysics;
}

void PhysicsEntity::SetDisableGravity(bool disableGravity) {
  m_disableGravity = disableGravity;
}

void PhysicsEntity::SetDisableCollision(bool disableCollision) {
  m_disableCollision = disableCollision;
}

void PhysicsEntity::PhysicsUpdate(const World& world) {
  if (m_disablePhysics) return;

  m_grounded = false;

  if (!m_disableGravity) {
    m_velocity.y += GetGravity() * Time::deltaTime;
    m_velocity.y = std::max(m_velocity.y, -50.0);
  }

  glm::dvec3 pos = GetPosition();
  BoundingBox boundingBox = GetBoundingBox();
  glm::dvec3 frameVelocity = m_velocity * Time::deltaTime;

  if (!m_disableCollision) {
    // You could slide up to three times (3 axes)
    for (int its = 0; its < 3; its++) {
      AABB entityAABB = AABB::CreateFromBottomCenter(pos, boundingBox.width, boundingBox.height);
      AxisRanges blockRanges = entityAABB.CalculateBlocksInsideBox(frameVelocity);

      float nearestTime = 1.0f;
      glm::dvec3 nearestNormal = { 0.0, 0.0, 0.0 };
      bool collided = false;

      // find nearest hit
      for (int x = blockRanges.minX; x <= blockRanges.maxX; x++) {
        for (int y = blockRanges.minY; y <= blockRanges.maxY; y++) {
          for (int z = blockRanges.minZ; z <= blockRanges.maxZ; z++) {
            if (Block::FromBlockstate(world.GetBlockstateAt(x, y, z)).IsAir()) continue;
            AABB blockAABB = AABB::CreateFromMinCorner({ x, y, z }, 1.0, 1.0);
            SweptCollisionResult result = entityAABB.SweptCollisionDetection(frameVelocity, blockAABB);

            if (result.time < nearestTime) {
              nearestTime = result.time;
              nearestNormal = result.normal;
              collided = true;
            }
          }
        }
      }

      // resolve the collision
      if (collided && nearestTime < 1.0f) {
        float safeTime = std::max(nearestTime - 0.001f, 0.0f);

        pos += frameVelocity * static_cast<double>(safeTime);

        glm::dvec3 originalVelocity = frameVelocity;
        float remainingTime = 1.0f - nearestTime;

        // handle the displacement (position change)
        glm::dvec3 remainingVelocity = originalVelocity * static_cast<double>(remainingTime);
        double dotprod = glm::dot(remainingVelocity, nearestNormal);
        glm::dvec3 slideVelocity = remainingVelocity - (dotprod * nearestNormal);
        frameVelocity = slideVelocity;

        // update the velocity
        double velocityDot = glm::dot(m_velocity, nearestNormal);
        m_velocity -= (velocityDot * nearestNormal);

        // check if the collision was against the ground
        if (nearestNormal.y > 0.0f) {
          m_grounded = true;
        }
      } else {
        break;
      }
    }
  }

  if (abs(frameVelocity.x) < 0.0001) {
    frameVelocity.x = 0.0;
  }
  if (abs(frameVelocity.y) < 0.0001) {
    frameVelocity.y = 0.0;
  }
  if (abs(frameVelocity.z) < 0.0001) {
    frameVelocity.z = 0.0;
  }

  pos += frameVelocity;

  SetPosition(pos);
}


bool PhysicsEntity::Jump() {
  if (!m_grounded) return false;

  m_velocity.y = GetJumpForce();
  return true;
}
