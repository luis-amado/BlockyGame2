#pragma once

#include "rendering/meshes/LineMesh.h"

struct BoundingBox {

  double width, height;

  LineMesh GetDebugMesh() const;

};