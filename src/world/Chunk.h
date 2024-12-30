#pragma once

#include <glm/vec2.hpp>
#include <vector>
#include <optional>
#include "util/ClassMacros.h"
#include "rendering/Mesh.h"
#include "rendering/Shader.h"

class World;

class Chunk {
public:
  // I really should not be copying chunks
  DELETE_COPY(Chunk);

  Chunk(glm::ivec2 chunkCoord, World& world);

  void GenerateTerrain();
  void GenerateMesh();

  void Draw(Shader& shader) const;
  bool GetBlockstateAt(int localX, int localY, int localZ) const;

  static const int CHUNK_WIDTH;
  static const int CHUNK_HEIGHT;
  static const int SUBCHUNK_HEIGHT;
  static const int SUBCHUNK_LAYERS;

private:
  glm::ivec2 m_chunkCoord;
  std::vector<bool> m_blockstates;
  std::vector<Mesh> m_subchunkMeshes;
  World& m_world;

  void GenerateMeshForSubchunk(int i);

  inline int PosToIndex(int localX, int localY, int localZ) const;
  bool IsInsideChunk(int localX, int localY, int localZ) const;
  glm::ivec3 ToGlobalCoords(int localX, int localY, int localZ) const;
};