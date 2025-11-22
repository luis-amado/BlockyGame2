#pragma once

#include "util/ClassMacros.h"
#include <vector>

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

class AttributeBuilder {
public:
  AttributeBuilder() = default;

  void AddAttribute(int size);
  void SetupAttributes(const VertexArray& vertexArray);

private:
  std::vector<int> m_attributeSizes;
};