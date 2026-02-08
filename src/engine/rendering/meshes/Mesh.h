#pragma once

#include "rendering/buffers/VertexArray.h"
#include "rendering/buffers/VertexBuffer.h"
#include "rendering/buffers/IndexBuffer.h"

class Mesh {
public:

  Mesh();
  virtual ~Mesh();

  DELETE_COPY(Mesh);

  Mesh(Mesh&& other);
  Mesh& operator=(Mesh&& other);

  void SetData(float* vertices, size_t vertexCount, unsigned int* indices, size_t indexCount);
  void Bind() const;
  virtual void Draw() const;

  bool HasData() const;

protected:
  VertexArray m_vertexArray;
  VertexBuffer m_vertexBuffer;
  IndexBuffer m_indexBuffer;

  unsigned int m_indexCount;
  bool m_hasData;

  virtual void SetupAttributes() const;
};