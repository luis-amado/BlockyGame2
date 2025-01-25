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

Mesh::Mesh()
  : m_hasData(false) {}

Mesh::Mesh(float* vertices, size_t vertexCount, unsigned int* indices, size_t indexCount)
  : m_vertexArray(),
  m_vertexBuffer(vertices, vertexCount * sizeof(float)),
  m_indexBuffer(indices, indexCount * sizeof(unsigned int)),
  m_indexCount(indexCount),
  m_hasData(true) {

  SetupAttributes();
}

void Mesh::SetData(float* vertices, size_t vertexCount, unsigned int* indices, size_t indexCount) {
  Bind();
  m_vertexBuffer.SetData(vertices, vertexCount * sizeof(float));
  m_indexBuffer.SetData(indices, indexCount * sizeof(unsigned int));
  m_indexCount = indexCount;
  m_hasData = true;

  SetupAttributes();
}

void Mesh::Bind() const {
  m_vertexArray.Bind();
}

void Mesh::Draw() const {
  // Don't draw mesh if it's not got any indices
  if (m_indexCount == 0) return;

  Bind();
  glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, NULL);
}

bool Mesh::HasData() const {
  return m_hasData;
}

void Mesh::SetupAttributes() const {
  // 3D Mesh is setup with vertex positions and texture coordinates
  m_vertexArray.AddAttribute(0, 3, 6 * sizeof(float), 0);
  m_vertexArray.AddAttribute(1, 2, 6 * sizeof(float), 3 * sizeof(float));
  m_vertexArray.AddAttribute(2, 1, 6 * sizeof(float), 5 * sizeof(float));
}
