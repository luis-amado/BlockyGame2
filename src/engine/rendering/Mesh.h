#pragma once

#include "buffers/VertexArray.h"
#include "buffers/VertexBuffer.h"
#include "buffers/IndexBuffer.h"

class Mesh {
public:


  Mesh(float* vertices, size_t vertexCount, unsigned int* indices, size_t indexCount);

  DELETE_COPY(Mesh);

  Mesh(Mesh&& other);
  Mesh& operator=(Mesh&& other);

  void Bind() const;
  void Draw() const;

private:
  VertexArray m_vertexArray;
  VertexBuffer m_vertexBuffer;
  IndexBuffer m_indexBuffer;

  unsigned int m_indexCount;
};