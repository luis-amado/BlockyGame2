#pragma once

#include <thread>
#include <unordered_map>
#include <glm/vec2.hpp>
#include "util/GlmExtensions.h"
#include "util/ClassMacros.h"
#include "util/threadsafe/ThreadSafePriorityQueue.h"
#include "util/threadsafe/ThreadSafeQueue.h"
#include "util/threadsafe/ThreadSafeUnorderedMap.h"
#include "rendering/Shader.h"
#include "Chunk.h"

using DistanceToChunk = std::pair<int, Chunk*>;

class World {
public:
  DELETE_COPY(World);

  World();
  ~World();

  void Start();
  void Update(glm::vec3 playerPosition);

  int GetChunksToGenerateTerrainSize() const;
  int GetChunksToGenerateMeshSize() const;

  bool GetBlockstateAt(int globalX, int globalY, int globalZ) const;
  glm::ivec2 GetChunkCoord(int globalX, int globalZ) const;
  glm::ivec3 ToLocalCoords(int globalX, int globalY, int globalZ) const;

  void Draw(Shader& shader) const;
private:
  ThreadSafeUnorderedMap<glm::ivec2, Chunk*, IVec2Hash, IVec2Equal> m_chunks;

  ThreadSafePriorityQueue<DistanceToChunk> m_chunkGenerationQueue;
  ThreadSafeQueue<Chunk*> m_chunksToGenerateTerrain;
  ThreadSafeQueue<Chunk*> m_chunksToGenerateMesh;
  ThreadSafeQueue<Chunk*> m_chunksToApplyMesh;

  std::vector<std::thread> m_workerThreads;

  Chunk* GetOrCreateChunkAt(glm::ivec2 chunkCoord);
  Chunk* GetChunkAt(glm::ivec2 chunkCoord) const;

  bool IsInsideWorld(int globalX, int globalY, int globalZ) const;

  friend void chunkTerrainGeneratorWorker(World& world, int workerId);
  friend void chunkMeshGeneratorWorker(World& world, int workerId);
};