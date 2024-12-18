#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(unsigned int* indices, size_t size) {
  glGenBuffers(1, &m_ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
}

IndexBuffer::~IndexBuffer() {
  if (m_ebo != 0) {
    glDeleteBuffers(1, &m_ebo);
  }
}

void IndexBuffer::Bind() const {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
}

void IndexBuffer::Unbind() const {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
