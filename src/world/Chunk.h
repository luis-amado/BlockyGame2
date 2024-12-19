#pragma once

#include <glm/vec2.hpp>
#include <vector>
#include <optional>
#include "util/ClassMacros.h"
#include "rendering/Mesh.h"
#include "rendering/Shader.h"

class Chunk {
public:
  // I really should not be copying chunks
  DELETE_COPY(Chunk);

  Chunk(glm::ivec2 chunkCoord);
  void Draw(Shader& shader) const;
  bool GetBlockstateAt(int x, int y, int z) const;

  static const int CHUNK_WIDTH;
  static const int CHUNK_HEIGHT;

private:
  glm::ivec2 m_chunkCoord;
  std::vector<bool> blockstates;
  std::optional<Mesh> m_mesh = std::nullopt;

  void GenerateTerrain();
  void GenerateMesh();

  inline int PosToIndex(int x, int y, int z) const;
  bool IsInsideChunk(int x, int y, int z) const;
};