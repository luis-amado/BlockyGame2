#pragma once

#include "util/ClassMacros.h"

class VertexArray {
public:
  ONLY_MOVE_ID(VertexArray, m_vao);

  VertexArray();
  ~VertexArray();

  void Bind() const;
  void Unbind() const;

  void AddAttribute(unsigned int index, int size, int stride, size_t offset) const;

private:
  unsigned int m_vao;
};