#include "BoundingBox.h"

LineMesh BoundingBox::GetDebugMesh() const {
  LineMesh mesh;

  float w = static_cast<float>(width);
  float h = static_cast<float>(height);
  float z = 0.0f;

  float vertices[] = {
    // pos // col
    z,z,z, 1.0f, 1.0f, 1.0f,
    w,z,z, 1.0f, 1.0f, 1.0f,
    w,z,w, 1.0f, 1.0f, 1.0f,
    z,z,w, 1.0f, 1.0f, 1.0f,
    z,h,z, 1.0f, 1.0f, 1.0f,
    w,h,z, 1.0f, 1.0f, 1.0f,
    w,h,w, 1.0f, 1.0f, 1.0f,
    z,h,w, 1.0f, 1.0f, 1.0f,
  };

  unsigned int indices[] = {
    0, 1,
    1, 2,
    2, 3,
    3, 0,
    4, 5,
    5, 6,
    6, 7,
    7, 4,
    0, 4,
    1, 5,
    2, 6,
    3, 7,
  };

  mesh.SetData(vertices, 48, indices, 24);
  return mesh;
}