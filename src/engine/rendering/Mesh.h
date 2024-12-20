#pragma once

#include "buffers/VertexArray.h"
#include "buffers/VertexBuffer.h"
#include "buffers/IndexBuffer.h"

class Mesh {
public:

  Mesh();
  Mesh(float* vertices, size_t vertexCount, unsigned int* indices, size_t indexCount);

  DELETE_COPY(Mesh);

  Mesh(Mesh&& other);
  Mesh& operator=(Mesh&& other);

  void SetData(float* vertices, size_t vertexCount, unsigned int* indices, size_t indexCount);
  void Bind() const;
  void Draw() const;

  bool HasData() const;

private:
  VertexArray m_vertexArray;
  VertexBuffer m_vertexBuffer;
  IndexBuffer m_indexBuffer;

  unsigned int m_indexCount;
  bool m_hasData;

  void SetupAttributes() const;
};