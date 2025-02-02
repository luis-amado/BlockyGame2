#pragma once

#include <glm/vec2.hpp>
#include <vector>
#include <optional>
#include <atomic>
#include "util/ClassMacros.h"
#include "rendering/Mesh.h"
#include "rendering/Shader.h"

class World;

struct MeshData {
  std::vector<float> vertices;
  std::vector<unsigned int> indices;
};

class Chunk {
public:
  // I really should not be copying chunks
  DELETE_COPY(Chunk);

  Chunk(glm::ivec2 chunkCoord, World& world);

  void GenerateTerrain();
  void GenerateMesh();
  void PropagateLighting();
  void ApplyMesh();

  void Draw(Shader& shader) const;
  char GetBlockstateAt(int localX, int localY, int localZ);
  char GetLightAt(int localX, int localY, int localZ);
  float GetFixedLightAt(int localX, int localY, int localZ);
  void SetLightAt(int localX, int localY, int localZ, char value);

  void SetActive(bool value);

  bool HasAppliedMesh() const;
  bool HasGeneratedTerrain() const;
  bool HasGeneratedMesh() const;
  bool HasPropagatedLighting() const;

  glm::ivec2 GetChunkCoord() const;

  static const int CHUNK_WIDTH;
  static const int CHUNK_HEIGHT;
  static const int SUBCHUNK_HEIGHT;
  static const int SUBCHUNK_LAYERS;

  bool m_queuedGeneration = false;

  std::atomic<bool> a_queuedTerrain = false;
  std::atomic<bool> a_queuedMesh = false;
  std::atomic<bool> a_queuedLighting = false;

private:
  std::vector<Chunk*> m_neighbors;
  glm::ivec2 m_chunkCoord;
  std::vector<char> m_blockstates;
  std::vector<char> m_lights;
  std::vector<Mesh> m_subchunkMeshes;
  std::vector<MeshData> m_subchunkMeshesData;
  World& m_world;

  bool m_generatedTerrain = false;
  bool m_propagatedLighting = false;
  bool m_generatedMesh = false;
  bool m_appliedMesh = false;

  bool m_active = false;

  void GenerateMeshForSubchunk(int i);
  void LightDFS(int x, int y, int z, char value);
  int GetNeighborIndex(int localX, int localZ) const;
  Chunk* GetNeighbor(int localX, int localZ);
  glm::ivec3 ToNeighborCoords(int localX, int localY, int localZ) const;

  inline int PosToIndex(int localX, int localY, int localZ) const;
  bool IsInsideChunk(int localX, int localY, int localZ) const;
  glm::ivec3 ToGlobalCoords(int localX, int localY, int localZ) const;
};