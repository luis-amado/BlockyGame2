#include <glad/glad.h>
#include "LineMesh.h"

void LineMesh::Draw() const {
  if (m_indexCount == 0) return;

  Bind();
  glDrawElements(GL_LINES, m_indexCount, GL_UNSIGNED_INT, NULL);
}

void LineMesh::SetupAttributes() const {
  AttributeBuilder builder;
  builder.AddAttribute(3); // position
  builder.SetupAttributes(m_vertexArray);
}