#pragma once

#include "util/ClassMacros.h"

class IndexBuffer {
public:
  ONLY_MOVE_ID(IndexBuffer, m_ebo);

  IndexBuffer();
  IndexBuffer(unsigned int* indices, size_t size);
  ~IndexBuffer();

  void SetData(unsigned int* indices, size_t size) const;

  void Bind() const;
  void Unbind() const;

private:
  unsigned int m_ebo;
};