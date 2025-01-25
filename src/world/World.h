#pragma once

#include <thread>
#include <unordered_map>
#include <queue>
#include <glm/vec2.hpp>
#include "util/GlmExtensions.h"
#include "util/ClassMacros.h"
#include "util/threadsafe/ThreadSafeQueue.h"
#include "util/threadsafe/ThreadSafeUnorderedMap.h"
#include "rendering/Shader.h"
#include "Chunk.h"
#include "../engine/camera/Camera.h"

using DistanceToChunk = std::pair<int, Chunk*>;

class World {
public:
  DELETE_COPY(World);

  World(const Camera& camera);
  ~World();

  void Start();
  void Stop();
  void Update();

  void ResetWorkers();
  void Regenerate();

  int GetChunksToGenerateTerrainSize() const;
  int GetChunksToLightSize() const;
  int GetChunksToGenerateMeshSize() const;

  char GetBlockstateAt(int globalX, int globalY, int globalZ) const;
  char GetLightAt(int globalX, int globalY, int globalZ) const;
  void SetLightAt(int globalX, int globalY, int globalZ, char value);
  glm::ivec2 GetChunkCoord(int globalX, int globalZ) const;
  glm::ivec3 ToLocalCoords(int globalX, int globalY, int globalZ) const;

  Chunk* GetChunkAtBlockPos(int globalX, int globalZ) const;

  void Draw(Shader& shader) const;
private:
  ThreadSafeUnorderedMap<glm::ivec2, Chunk*, IVec2Hash, IVec2Equal> m_chunks;

  std::priority_queue<DistanceToChunk, std::vector<DistanceToChunk>, std::greater<DistanceToChunk>> m_chunkGenerationQueue;
  ThreadSafeQueue<Chunk*> m_chunksToGenerateTerrain;
  ThreadSafeQueue<Chunk*> m_chunksToPropagateLighting;
  ThreadSafeQueue<Chunk*> m_chunksToGenerateMesh;
  ThreadSafeQueue<Chunk*> m_chunksToApplyMesh;

  std::vector<std::thread> m_workerThreads;
  const Camera& m_camera;

  Chunk* GetOrCreateChunkAt(glm::ivec2 chunkCoord);
  Chunk* GetChunkAt(glm::ivec2 chunkCoord) const;

  bool IsInsideWorld(int globalX, int globalY, int globalZ) const;

  friend void chunkTerrainGeneratorWorker(World& world, int workerId);
  friend void chunkMeshGeneratorWorker(World& world, int workerId);
  friend void chunkLightingWorker(World& world, int workerId);
};