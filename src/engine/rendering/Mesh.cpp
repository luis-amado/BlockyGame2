#include <glad/glad.h>

#include "Mesh.h"

Mesh::Mesh(Mesh&& other)
  : m_vertexArray(std::move(other.m_vertexArray)),
  m_vertexBuffer(std::move(other.m_vertexBuffer)),
  m_indexBuffer(std::move(other.m_indexBuffer)),
  m_indexCount(other.m_indexCount) {}

Mesh& Mesh::operator=(Mesh&& other) {
  m_vertexArray = std::move(other.m_vertexArray);
  m_vertexBuffer = std::move(other.m_vertexBuffer);
  m_indexBuffer = std::move(other.m_indexBuffer);
  m_indexCount = other.m_indexCount;

  return *this;
}

Mesh::Mesh(float* vertices, size_t vertexCount, unsigned int* indices, size_t indexCount)
  : m_vertexArray(),
  m_vertexBuffer(vertices, vertexCount * sizeof(float)),
  m_indexBuffer(indices, indexCount * sizeof(unsigned int)),
  m_indexCount(indexCount) {

  // 3D Mesh is setup with vertex positions and texture coordinates
  m_vertexArray.AddAttribute(0, 3, 5 * sizeof(float), 0);
  m_vertexArray.AddAttribute(1, 2, 5 * sizeof(float), 3 * sizeof(float));

}

void Mesh::Bind() const {
  m_vertexArray.Bind();
}

void Mesh::Draw() const {
  Bind();
  glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, NULL);
}
