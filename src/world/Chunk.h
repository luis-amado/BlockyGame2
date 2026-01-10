#pragma once

#include <glm/vec2.hpp>
#include <vector>
#include <optional>
#include <unordered_set>
#include <atomic>
#include "util/ClassMacros.h"
#include "rendering/Mesh.h"
#include "rendering/Shader.h"
#include <shared_mutex>
#include "../init/Blocks.h"
#include "util/GlmExtensions.h"

class World;

struct MeshData {
  std::vector<float> vertices;
  std::vector<unsigned int> indices;
};

// TODO: Abstract lighting to a separate file

enum class LightType {
  SKY, BLOCK
};

struct SkyBlockLight {
  // char is 8 bits, a light value is only 4 bits
  // first 4 bits are sky, last 4 bits are block
  unsigned char m_value;

  char GetLight(LightType type);
  void SetLight(LightType type, char value);
};

using PositionsToSpreadLightMap = std::unordered_map<glm::ivec3, char, IVec3Hash, IVec3Equal>;

class Chunk : public std::enable_shared_from_this<Chunk> {
public:
  DELETE_COPY(Chunk);

  Chunk(glm::ivec2 chunkCoord, World& world);

  void GenerateTerrain();
  void GenerateMesh();
  void PropagateLighting();
  void PropagateLightingAtPos(glm::ivec3 localPosition, Blockstate oldBlockstate, Blockstate newBlockstate);
  void FillSkyLight();
  void ApplyMesh();

  // void UpdateMeshAtPosition(glm::ivec3 position);

  // Indicate that the mesh at a certain position is not accurate anymore
  void MarkPositionDirty(glm::ivec3 localPosition);
  void MarkPositionAndNeighborsDirty(glm::ivec3 localPosition);
  void MarkPositionAndAllNeighborsDirty(glm::ivec3 localPosition);
  // Recalculate and apply the meshes at dirty subchunks
  void CleanDirty();

  void Draw(Shader& shader) const;
  Blockstate GetBlockstateAt(int localX, int localY, int localZ);
  const Block& GetBlockAt(int localX, int localY, int localZ);
  char GetLightAt(LightType type, int localX, int localY, int localZ);
  float GetFixedLightAt(LightType type, int localX, int localY, int localZ);
  void SetLightAt(LightType type, int localX, int localY, int localZ, char value);
  void SetBlockstateAt(int localX, int localY, int localZ, Blockstate value);

  void SetActive(bool value);

  void InvalidateMesh();

  bool HasAppliedMesh() const;
  bool HasGeneratedTerrain() const;
  bool HasGeneratedMesh() const;
  bool HasPropagatedLighting() const;

  glm::ivec2 GetChunkCoord() const;
  std::shared_ptr<Chunk> GetNeighbor(int localX, int localZ);
  int GetNeighborIndex(int localX, int localZ) const;

  static const int CHUNK_WIDTH;
  static const int CHUNK_HEIGHT;
  static const int SUBCHUNK_HEIGHT;
  static const int SUBCHUNK_LAYERS;

  bool m_queuedGeneration = false;

  std::atomic<bool> a_queuedTerrain = false;
  std::atomic<bool> a_queuedMesh = false;
  std::atomic<bool> a_queuedLighting = false;

private:
  std::vector<std::weak_ptr<Chunk>> m_neighbors;
  glm::ivec2 m_chunkCoord;

  std::vector<Blockstate> m_blockstates;
  std::vector<SkyBlockLight> m_lights;

  std::vector<Mesh> m_subchunkMeshes;
  std::vector<MeshData> m_subchunkMeshesData;
  std::unordered_set<int> m_dirtySubchunks;
  World& m_world;

  // TODO: Make a state enum variable
  mutable std::mutex m_stateMutex;
  bool m_generatedTerrain = false;
  bool m_propagatedLighting = false;
  bool m_generatedMesh = false;
  bool m_appliedMesh = false;

  bool m_active = false;

  void GenerateMeshForSubchunk(int i);

  void LightSpreadingDFS(LightType type, int x, int y, int z, char value, bool markDirty = false);
  void LightUpdatingDFS(LightType type, int x, int y, int z);
  void LightRemovingDFS(LightType type, int x, int y, int z, char value, char oldValue, PositionsToSpreadLightMap& positionsToSpreadAfter);
  glm::ivec3 ToNeighborCoords(int localX, int localY, int localZ) const;

  inline int PosToIndex(int localX, int localY, int localZ) const;
  inline int PosToIndex(const glm::ivec3& local) const;
  bool IsInsideChunk(int localX, int localY, int localZ) const;
  bool IsInOtherChunk(int localX, int localY, int localZ) const;
  glm::ivec3 ToGlobalCoords(int localX, int localY, int localZ) const;
  glm::ivec3 ToGlobalCoords(const glm::ivec3& local) const;
  int GetSubchunkIndex(int localY) const;
};