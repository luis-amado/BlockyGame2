#pragma once

#include "../physics/BoundingBox.h"
#include <glm/vec3.hpp>

namespace DebugShapes {

void DrawBoundingBox(BoundingBox boundingBox, glm::dvec3 position);
void DrawBlockBox(glm::ivec3 position, glm::vec3 color = { 1.0f, 1.0f, 1.0f });

}