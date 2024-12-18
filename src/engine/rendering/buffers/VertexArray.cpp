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
