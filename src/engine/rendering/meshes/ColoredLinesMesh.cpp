#include <glad/glad.h>
#include "ColoredLinesMesh.h"

void ColoredLinesMesh::Draw() const {
  if (m_indexCount == 0) return;

  Bind();
  glDrawElements(GL_LINES, m_indexCount, GL_UNSIGNED_INT, NULL);
}

void ColoredLinesMesh::SetupAttributes() const {
  AttributeBuilder builder;
  builder.AddAttribute(3); // position
  builder.AddAttribute(3); // color
  builder.SetupAttributes(m_vertexArray);
}
