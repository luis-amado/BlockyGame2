#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "VertexArray.h"

VertexArray::VertexArray() {
  glGenVertexArrays(1, &m_vao);
  glBindVertexArray(m_vao);
}

VertexArray::~VertexArray() {
  if (m_vao != 0) {
    glDeleteVertexArrays(1, &m_vao);
  }
}

void VertexArray::Bind() const {
  glBindVertexArray(m_vao);
}

void VertexArray::Unbind() const {
  glBindVertexArray(0);
}

void VertexArray::AddAttribute(unsigned int index, int size, int stride, size_t offset) const {
  // Bind();
  glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, stride, (void*)offset);
  glEnableVertexAttribArray(index);
}

void AttributeBuilder::AddAttribute(int size) {
  m_attributeSizes.push_back(size);
}

void AttributeBuilder::SetupAttributes(const VertexArray& vertexArray) {
  int stride = 0;
  for (int size : m_attributeSizes) {
    stride += size;
  }

  int index = 0;
  size_t offset = 0;
  for (int size : m_attributeSizes) {
    vertexArray.AddAttribute(index++, size, stride * sizeof(float), offset * sizeof(float));
    offset += size;
  }
}
