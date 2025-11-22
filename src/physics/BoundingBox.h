#pragma once

#include "../engine/rendering/LineMesh.h"

struct BoundingBox {

  double width, height;

  LineMesh GetDebugMesh() const;

};