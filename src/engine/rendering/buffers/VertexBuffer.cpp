#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "VertexBuffer.h"
#include "ResourceGraveyard.h"

VertexBuffer::VertexBuffer() {
  glGenBuffers(1, &m_vbo);
}

VertexBuffer::VertexBuffer(float* vertices, size_t size) {
  glGenBuffers(1, &m_vbo);
  // glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  // glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
  SetData(vertices, size);
}

VertexBuffer::~VertexBuffer() {
  if (m_vbo != 0) {
    ResourceGraveyard::GetInstance().QueueVBOForDeletion(m_vbo);
    m_vbo = 0;
  }
}

void VertexBuffer::SetData(float* vertices, size_t size) const {
  Bind();
  glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

void VertexBuffer::Bind() const {
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
}

void VertexBuffer::Unbind() const {
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}
