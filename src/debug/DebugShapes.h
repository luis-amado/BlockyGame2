#pragma once

#include "../physics/BoundingBox.h"
#include <glm/vec3.hpp>

namespace DebugShapes {

void DrawBoundingBox(BoundingBox boundingBox, glm::dvec3 position);
void DrawBlockBox(glm::ivec3 position, glm::vec3 color = { 1.0f, 1.0f, 1.0f });
void DrawLine(glm::dvec3 start, glm::dvec3 end, glm::vec3 color = { 1.0f, 1.0f, 1.0f });

}