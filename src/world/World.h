#pragma once

#include <thread>
#include <unordered_map>
#include <queue>
#include <glm/vec2.hpp>
#include "util/GlmExtensions.h"
#include "util/ClassMacros.h"
#include "util/threadsafe/ThreadSafeQueue.h"
#include "util/threadsafe/ThreadSafeUnorderedMap.h"
#include "util/threadsafe/ThreadSafeReference.h"
#include "rendering/Shader.h"
#include "Chunk.h"
#include "../entity/Entity.h"

using DistanceToChunk = std::pair<int, Chunk*>;

class World {
public:
  DELETE_COPY(World);

  World(const Entity& camera);
  ~World();

  void Start();
  void Stop();
  void Update();

  void ResetWorkers();
  void Regenerate();

  int GetChunkCount() const;
  int GetChunksToGenerateTerrainSize() const;
  int GetChunksToLightSize() const;
  int GetChunksToGenerateMeshSize() const;

  void MarkChunkDirty(Chunk* chunk);

  void MarkPositionDirty(const glm::ivec3& globalPosition) const;
  char GetBlockstateAt(int globalX, int globalY, int globalZ) const;
  char GetBlockstateAt(const glm::ivec3& globalCoords) const;
  char GetLightAt(int globalX, int globalY, int globalZ) const;
  void SetLightAt(int globalX, int globalY, int globalZ, char value);
  glm::ivec2 GetChunkCoord(int globalX, int globalZ) const;
  glm::ivec3 ToLocalCoords(int globalX, int globalY, int globalZ) const;
  glm::ivec3 ToLocalCoords(glm::ivec3 global) const;

  void UpdateBlockstateAt(int globalX, int globalY, int globalZ, char blockstate);

  Chunk* GetChunkAtBlockPos(int globalX, int globalZ) const;
  ThreadSafeReference<Chunk> GetChunkRefAtBlockPos(int globalX, int globalZ) const;

  void CleanDirtyChunks();

  void Draw() const;
private:
  ThreadSafeUnorderedMap<glm::ivec2, Chunk*, IVec2Hash, IVec2Equal> m_chunks;

  std::priority_queue<DistanceToChunk, std::vector<DistanceToChunk>, std::greater<DistanceToChunk>> m_chunkGenerationQueue;
  ThreadSafeQueue<glm::ivec2> m_chunksToGenerateTerrain;
  ThreadSafeQueue<glm::ivec2> m_chunksToPropagateLighting;
  ThreadSafeQueue<glm::ivec2> m_chunksToGenerateMesh;
  ThreadSafeQueue<glm::ivec2> m_chunksToApplyMesh;

  // Chunks to immediately remesh (caller function must have caused the dirtyness)
  std::unordered_set<Chunk*> m_dirtyChunks;

  std::vector<std::thread> m_workerThreads;
  const Entity& m_trackingEntity;

  Chunk* GetOrCreateChunkAt(glm::ivec2 chunkCoord);
  Chunk* GetChunkAt(glm::ivec2 chunkCoord) const;
  ThreadSafeReference<Chunk> GetChunkRefAt(glm::ivec2 chunkCoord) const;

  bool IsInsideWorld(int globalX, int globalY, int globalZ) const;

  friend void chunkTerrainGeneratorWorker(World& world, int workerId);
  friend void chunkMeshGeneratorWorker(World& world, int workerId);
  friend void chunkLightingWorker(World& world, int workerId);
};