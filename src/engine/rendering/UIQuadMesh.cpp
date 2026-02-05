#include "UIQuadMesh.h"

void UIQuadMesh::SetupAttributes() const {
  AttributeBuilder builder;
  // texture coordinates can be identified from position (they're the same)
  builder.AddAttribute(2); // position
  builder.SetupAttributes(m_vertexArray);
}