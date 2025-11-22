#pragma once

#include "../physics/BoundingBox.h"
#include <glm/vec3.hpp>

namespace DebugShapes {

void DrawBoundingBox(BoundingBox boundingBox, glm::dvec3 position);

}