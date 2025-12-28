#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "ResourceGraveyard.h"

#include "IndexBuffer.h"

IndexBuffer::IndexBuffer() {
  glGenBuffers(1, &m_ebo);
}

IndexBuffer::IndexBuffer(unsigned int* indices, size_t size) {
  glGenBuffers(1, &m_ebo);
  // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
  // glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
  SetData(indices, size);
}

IndexBuffer::~IndexBuffer() {
  if (m_ebo != 0) {
    ResourceGraveyard::GetInstance().QueueVBOForDeletion(m_ebo);
    m_ebo = 0;
  }
}

void IndexBuffer::SetData(unsigned int* indices, size_t size) const {
  Bind();
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
}

void IndexBuffer::Bind() const {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
}

void IndexBuffer::Unbind() const {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
