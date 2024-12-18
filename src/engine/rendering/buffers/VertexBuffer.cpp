#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(float* vertices, size_t size) {
  glGenBuffers(1, &m_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer() {
  if (m_vbo != 0) {
    glDeleteBuffers(1, &m_vbo);
  }
}

void VertexBuffer::Bind() const {
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
}

void VertexBuffer::Unbind() const {
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}
