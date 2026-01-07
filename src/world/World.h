#pragma once

#include <thread>
#include <unordered_map>
#include <queue>
#include <glm/vec2.hpp>
#include "util/GlmExtensions.h"
#include "util/ClassMacros.h"
#include "util/threadsafe/ThreadSafeQueue.h"
#include "util/threadsafe/ThreadSafePriorityQueue.h"
#include "util/threadsafe/ThreadSafeUnorderedMap.h"
#include "rendering/Shader.h"
#include "Chunk.h"
#include "../entity/Entity.h"
#include "../init/Blocks.h"

using DistanceToChunk = std::pair<long, Chunk*>;

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
  const Entity& GetTrackingEntity() const;

  void MarkChunkDirty(Chunk* chunk);

  void MarkPositionDirty(const glm::ivec3& globalPosition) const;
  Blockstate GetBlockstateAt(int globalX, int globalY, int globalZ) const;
  const Block& GetBlockAt(int globalX, int globalY, int globalZ) const;
  char GetLightAt(LightType type, int globalX, int globalY, int globalZ) const;
  void SetLightAt(LightType type, int globalX, int globalY, int globalZ, char value);
  glm::ivec2 GetChunkCoord(int globalX, int globalZ) const;
  glm::ivec3 ToLocalCoords(int globalX, int globalY, int globalZ) const;
  glm::ivec3 ToLocalCoords(glm::ivec3 global) const;

  void RemoveChunk(glm::ivec2 chunkCoord);

  void UpdateBlockstateAt(int globalX, int globalY, int globalZ, Blockstate blockstate);

  std::shared_ptr<Chunk> GetChunkAtBlockPos(int globalX, int globalZ) const;

  void CleanDirtyChunks();
  void RemeshAllChunks();

  void Draw() const;

  std::shared_ptr<Chunk> GetChunkAt(glm::ivec2 chunkCoord) const;
private:
  ThreadSafeUnorderedMap<glm::ivec2, std::shared_ptr<Chunk>, IVec2Hash, IVec2Equal> m_chunks;

  std::priority_queue<DistanceToChunk, std::vector<DistanceToChunk>, std::greater<DistanceToChunk>> m_chunkGenerationQueue;
  ThreadSafeQueue<std::weak_ptr<Chunk>> m_chunksToGenerateTerrain;
  ThreadSafeQueue<std::weak_ptr<Chunk>> m_chunksToPropagateLighting;
  ThreadSafeQueue<std::weak_ptr<Chunk>> m_chunksToGenerateMesh;
  ThreadSafeQueue<std::weak_ptr<Chunk>> m_chunksToApplyMesh;

  // Chunks to immediately remesh (caller function must have caused the dirtyness)
  std::unordered_set<Chunk*> m_dirtyChunks;

  std::vector<std::thread> m_workerThreads;
  const Entity& m_trackingEntity;

  std::shared_ptr<Chunk> GetOrCreateChunkAt(glm::ivec2 chunkCoord);

  bool IsInsideWorld(int globalX, int globalY, int globalZ) const;

  friend void chunkTerrainGeneratorWorker(World& world, int workerId);
  friend void chunkMeshGeneratorWorker(World& world, int workerId);
  friend void chunkLightingWorker(World& world, int workerId);
};