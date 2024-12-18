#pragma once

#include "util/ClassMacros.h"

class VertexBuffer {
public:
  ONLY_MOVE_ID(VertexBuffer, m_vbo);

  VertexBuffer(float* vertices, size_t size);
  ~VertexBuffer();

  void Bind() const;
  void Unbind() const;

private:
  unsigned int m_vbo;
};