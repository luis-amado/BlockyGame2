#include <memory>
#include <thread>
#include <functional>
#include <unordered_set>

#include <glad/glad.h>

#include "World.h"
#include "Chunk.h"
#include "util/MathUtil.h"
#include "util/Logging.h"
#include "../engine/rendering/ShaderLibrary.h"
#include "../engine/rendering/buffers/ResourceGraveyard.h"
#include "../voxel/VoxelData.h"

#include "../debug/DebugSettings.h"
#include "../init/Blocks.h"

namespace {
glm::ivec2 chunkOffsetsWithCorners[] = { {0, 0}, {0, 1}, {1, 0}, {0, -1}, {-1, 0}, {-1, -1}, {1, -1}, {1, 1}, {-1, 1} };

long GetDistanceToChunk(int diffX, int diffZ) {
  return std::abs(diffX) * std::abs(diffX) + std::abs(diffZ) * std::abs(diffZ);
}

} // namespace

void chunkTerrainGeneratorWorker(World& world, int workerId) {

  while (true) {
    std::weak_ptr<Chunk> weakChunk;
    if (!world.m_chunksToGenerateTerrain.pop(weakChunk)) break;

    // Chunks could have been deleted, so obtain a valid pointer if one exists
    if (auto chunk = weakChunk.lock()) {
      if (!chunk->HasGeneratedTerrain()) {
        chunk->GenerateTerrain();
      }

      // Check if a neighboring chunk, or this one, is ready for lighting
      glm::ivec2 centerCoord = chunk->GetChunkCoord();
      for (const glm::ivec2& offset : chunkOffsetsWithCorners) {
        glm::ivec2 chunkCoord = centerCoord + offset;

        // Check all the neighbors
        std::shared_ptr<Chunk> currChunk = world.GetChunkAt(chunkCoord);
        if (currChunk == nullptr) {
          continue;
        }

        bool ready = true;
        for (glm::ivec2& secondOffset : chunkOffsetsWithCorners) {
          glm::ivec2 secondChunkCoord = chunkCoord + secondOffset;

          std::shared_ptr<Chunk> neighborChunk = world.GetChunkAt(secondChunkCoord);
          if (neighborChunk == nullptr || !neighborChunk->HasGeneratedTerrain()) {
            ready = false;
            break;
          }
        }

        if (ready && !currChunk->a_queuedLighting.exchange(true)) {
          // long distance = GetDistanceToChunk(chunkCoord, world);
          world.m_chunksToPropagateLighting.push(currChunk);
        }

      }
    }

  }

  LOG(EXTRA) << "Chunk terrain generator worker #" << workerId << " stopped";
}

void chunkLightingWorker(World& world, int workerId) {
  while (true) {
    std::weak_ptr<Chunk> weakChunk;
    if (!world.m_chunksToPropagateLighting.pop(weakChunk)) break;

    if (auto chunk = weakChunk.lock()) {

      if (!chunk->HasPropagatedLighting()) {
        chunk->PropagateLighting();
      }

      // Check if a neighboring chunk, or this one, is ready for meshing
      glm::ivec2 centerCoord = chunk->GetChunkCoord();
      for (const glm::ivec2& offset : chunkOffsetsWithCorners) {
        glm::ivec2 chunkCoord = centerCoord + offset;

        std::shared_ptr<Chunk> currChunk = world.GetChunkAt(chunkCoord);
        if (currChunk == nullptr || currChunk->a_queuedMesh) continue;

        // Check all the neighbors
        bool ready = true;
        for (glm::ivec2& secondOffset : chunkOffsetsWithCorners) {
          glm::ivec2 secondChunkCoord = chunkCoord + secondOffset;

          std::shared_ptr<Chunk> neighborChunk = world.GetChunkAt(secondChunkCoord);
          if (neighborChunk == nullptr || !neighborChunk->HasPropagatedLighting()) {
            ready = false;
            break;
          }
        }

        if (ready && !currChunk->a_queuedMesh.exchange(true)) {
          // long distance = GetDistanceToChunk(chunkCoord, world);
          world.m_chunksToGenerateMesh.push(currChunk);
        }

      }
    }
  }

  LOG(EXTRA) << "Chunk lighting worker #" << workerId << " stopped";
}

void chunkMeshGeneratorWorker(World& world, int workerId) {
  while (true) {
    std::weak_ptr<Chunk> weakChunk;
    if (!world.m_chunksToGenerateMesh.pop(weakChunk)) break;

    if (auto chunk = weakChunk.lock()) {
      glm::ivec2 chunkCoord = chunk->GetChunkCoord();

      chunk->GenerateMesh();
      // long distance = GetDistanceToChunk(chunkCoord, world);
      world.m_chunksToApplyMesh.push(chunk);
    }
  }

  LOG(EXTRA) << "Chunk mesh generator worker #" << workerId << " stopped";
}

World::World(const Entity& trackingEntity) : m_trackingEntity(trackingEntity) {}

World::~World() {
  Stop();
}

void World::Start() {
  // Make sure the work queues are active
  m_chunksToGenerateMesh.start();
  m_chunksToGenerateTerrain.start();
  m_chunksToPropagateLighting.start();

  // Generate the worker threads
  for (int i = 0; i < DebugSettings::instance.terrainWorkerCount; i++) {
    m_workerThreads.push_back(std::thread([this, i]() {
      chunkTerrainGeneratorWorker(*this, i);
    }));
  }
  for (int i = 0; i < DebugSettings::instance.lightingWorkerCount; i++) {
    m_workerThreads.push_back(std::thread([this, i]() {
      chunkLightingWorker(*this, i);
    }));
  }
  for (int i = 0; i < DebugSettings::instance.meshWorkerCount; i++) {
    m_workerThreads.push_back(std::thread([this, i]() {
      chunkMeshGeneratorWorker(*this, i);
    }));
  }

}

void World::Stop() {
  // Stop the worker threads
  m_chunksToPropagateLighting.stop();
  m_chunksToGenerateTerrain.stop();
  m_chunksToGenerateMesh.stop();

  for (auto& thread : m_workerThreads) {
    if (thread.joinable()) {
      thread.join();
    }
  }
  m_workerThreads.clear();

  // Clear the work queues
  m_chunkGenerationQueue = {};
  m_chunksToGenerateTerrain.clear();
  m_chunksToPropagateLighting.clear();
  m_chunksToGenerateMesh.clear();
  m_chunksToApplyMesh.clear();

  // Clear the chunks
  m_chunks.clear();
  ResourceGraveyard::GetInstance().Flush();
}

void World::ResetWorkers() {
  m_chunksToGenerateTerrain.stop();
  m_chunksToPropagateLighting.stop();
  m_chunksToGenerateMesh.stop();

  for (auto& thread : m_workerThreads) {
    if (thread.joinable()) {
      thread.join();
    }
  }
  m_workerThreads.clear();

  Start();
}

void World::Update() {

  glm::ivec3 playerPosition = m_trackingEntity.GetPosition();
  glm::ivec2 playerChunk = GetChunkCoord((int)floor(playerPosition.x), (int)floor(playerPosition.z));

  int renderDistance = DebugSettings::instance.renderDistance;
  int inMemoryRadius = renderDistance + DebugSettings::instance.inMemoryBorder;

  std::vector<glm::ivec2> chunksCoordsToUnload;

  m_chunks.forEach([&](glm::ivec2 coord, std::shared_ptr<Chunk> chunk) {
    chunk->SetActive(false);

    if (std::abs(coord.x - playerChunk.x) > inMemoryRadius || std::abs(coord.y - playerChunk.y) > inMemoryRadius) {
      chunksCoordsToUnload.push_back(coord);
    }
  });

  int renderDistance2 = renderDistance * renderDistance;

  for (int x = -renderDistance; x <= renderDistance; x++) {
    for (int z = -renderDistance; z <= renderDistance; z++) {

      // Filter out chunks that are outside the render distance "circle"
      if (x * x + z * z >= renderDistance2) {
        continue;
      }

      glm::ivec2 chunkCoord(x, z);
      chunkCoord += playerChunk;

      std::shared_ptr<Chunk> chunk = GetOrCreateChunkAt(chunkCoord);
      chunk->SetActive(true);

      // Queue the chunk for generation if it hasnt been already queued
      if (!chunk->m_queuedGeneration) {
        long distance = GetDistanceToChunk(x, z);
        m_chunkGenerationQueue.push({ distance, chunk.get() });
        chunk->m_queuedGeneration = true;
      }
    }
  }

  // Go through the chunks that need to be generated
  while (!m_chunkGenerationQueue.empty()) {
    // Chunks that need to be generated in order to make the mesh for the chunk at 0, 0
    DistanceToChunk distanceToChunk = m_chunkGenerationQueue.top();
    m_chunkGenerationQueue.pop();

    Chunk* centerChunk = distanceToChunk.second;
    glm::ivec2 centerCoord = centerChunk->GetChunkCoord();

    // We need to generate the terrain for this chunk and all other adjacent chunks
    for (const glm::ivec2& offset : chunkOffsetsWithCorners) {
      glm::ivec2 chunkCoord = centerCoord + offset;
      std::shared_ptr<Chunk> chunk = GetOrCreateChunkAt(chunkCoord);

      if (!chunk->a_queuedTerrain.exchange(true)) {
        // long distance = GetDistanceToChunk(chunkCoord, playerChunk);
        m_chunksToGenerateTerrain.push(chunk);
      }
    }
  }

  while (!m_chunksToApplyMesh.empty()) {
    std::weak_ptr<Chunk> weakChunk;
    m_chunksToApplyMesh.pop(weakChunk);

    if (auto chunk = weakChunk.lock()) {
      chunk->ApplyMesh();
    }
  }

  for (glm::ivec2 coord : chunksCoordsToUnload) {
    m_chunks.erase(coord);
  }

  ResourceGraveyard::GetInstance().Flush();
}

void World::Regenerate() {
  // Reset all known information about the world
  Stop();
  Start();
}

int World::GetChunkCount() const {
  return m_chunks.size();
}

int World::GetChunksToGenerateTerrainSize() const {
  return m_chunksToGenerateTerrain.size();
}

int World::GetChunksToLightSize() const {
  return m_chunksToPropagateLighting.size();
}

int World::GetChunksToGenerateMeshSize() const {
  return m_chunksToGenerateMesh.size();
}

const Entity& World::GetTrackingEntity() const {
  return m_trackingEntity;
}

void World::MarkChunkDirty(Chunk* chunk) {
  m_dirtyChunks.insert(chunk);
}

void World::MarkPositionDirty(const glm::ivec3& globalPosition) const {
  GetChunkAtBlockPos(globalPosition.x, globalPosition.z)->MarkPositionDirty(ToLocalCoords(globalPosition));
}

char World::GetBlockstateAt(int globalX, int globalY, int globalZ) const {
  if (IsInsideWorld(globalX, globalY, globalZ)) {
    glm::ivec2 chunkCoord = GetChunkCoord(globalX, globalZ);
    glm::ivec3 localCoords = ToLocalCoords(globalX, globalY, globalZ);
    return GetChunkAt(chunkCoord)->GetBlockstateAt(localCoords.x, localCoords.y, localCoords.z);
  } else {
    return Blocks::VOID_AIR;
  }
}

char World::GetBlockstateAt(const glm::ivec3& globalCoords) const {
  return GetBlockstateAt(globalCoords.x, globalCoords.y, globalCoords.z);
}

char World::GetLightAt(int globalX, int globalY, int globalZ) const {
  if (IsInsideWorld(globalX, globalY, globalZ)) {
    glm::ivec2 chunkCoord = GetChunkCoord(globalX, globalZ);
    glm::ivec3 localCoords = ToLocalCoords(globalX, globalY, globalZ);
    return GetChunkAt(chunkCoord)->GetLightAt(localCoords.x, localCoords.y, localCoords.z);
  } else {
    return 0;
  }
}

void World::SetLightAt(int globalX, int globalY, int globalZ, char value) {
  if (IsInsideWorld(globalX, globalY, globalZ)) {
    glm::ivec2 chunkCoord = GetChunkCoord(globalX, globalZ);
    glm::ivec3 localCoords = ToLocalCoords(globalX, globalY, globalZ);
    GetChunkAt(chunkCoord)->SetLightAt(localCoords.x, localCoords.y, localCoords.z, value);
  }
}

void World::Draw() const {
  Shader& shader = ShaderLibrary::GetInstance().Get("main");
  shader.Use();
  shader.LoadBool("nightVision", DebugSettings::instance.nightVision);
  Blocks::GetAtlas().Use();
  m_chunks.forEach([&](glm::ivec2 coord, std::shared_ptr<Chunk> chunk) {
    chunk->Draw(shader);
  });
}

std::shared_ptr<Chunk> World::GetOrCreateChunkAt(glm::ivec2 chunkCoord) {
  std::shared_ptr<Chunk> chunk;

  // Create the chunk if it doesnt exist
  if (!m_chunks.contains(chunkCoord)) {
    chunk = std::make_shared<Chunk>(chunkCoord, *this);
    m_chunks.insert(chunkCoord, chunk);
  } else {
    chunk = m_chunks.get(chunkCoord).value();
  }

  return chunk;
}

std::shared_ptr<Chunk> World::GetChunkAt(glm::ivec2 chunkCoord) const {
  std::shared_ptr<Chunk> chunk = m_chunks.get(chunkCoord).value_or(nullptr);
  return chunk;
}

bool World::IsInsideWorld(int globalX, int globalY, int globalZ) const {
  if (globalY < 0 || globalY >= Chunk::CHUNK_HEIGHT) {
    return false;
  }
  glm::ivec2 chunkCoord = GetChunkCoord(globalX, globalZ);
  return m_chunks.contains(chunkCoord);
}

glm::ivec2 World::GetChunkCoord(int globalX, int globalZ) const {
  return {
    std::floor((float)globalX / Chunk::CHUNK_WIDTH),
    std::floor((float)globalZ / Chunk::CHUNK_WIDTH)
  };
}

glm::ivec3 World::ToLocalCoords(int globalX, int globalY, int globalZ) const {
  return {
    MathUtil::Mod(globalX, Chunk::CHUNK_WIDTH),
    globalY,
    MathUtil::Mod(globalZ, Chunk::CHUNK_WIDTH)
  };
}

glm::ivec3 World::ToLocalCoords(glm::ivec3 global) const {
  return {
    MathUtil::Mod(global.x, Chunk::CHUNK_WIDTH),
    global.y,
    MathUtil::Mod(global.z, Chunk::CHUNK_WIDTH)
  };
}

void World::RemoveChunk(glm::ivec2 chunkCoord) {
  m_chunks.erase(chunkCoord);
}

void World::UpdateBlockstateAt(int globalX, int globalY, int globalZ, char blockstate) {

  glm::ivec3 position = { globalX, globalY, globalZ };
  std::unordered_set<Chunk*> dirtyChunks;

  std::shared_ptr<Chunk> chunk = GetChunkAtBlockPos(globalX, globalZ);
  glm::ivec3 localCoords = ToLocalCoords(globalX, globalY, globalZ);
  chunk->SetBlockstateAt(localCoords.x, localCoords.y, localCoords.z, blockstate);

  // Mark block and neighbors as dirty (because they may need to adjust their meshes)
  chunk->MarkPositionAndNeighborsDirty(localCoords);

  // Recalculate lighting
  chunk->PropagateLightingAtPos(localCoords, 0);

  CleanDirtyChunks();
}

std::shared_ptr<Chunk> World::GetChunkAtBlockPos(int globalX, int globalZ) const {
  return m_chunks.get(GetChunkCoord(globalX, globalZ)).value_or(nullptr);
}

void World::CleanDirtyChunks() {
  for (Chunk* chunk : m_dirtyChunks) {
    chunk->CleanDirty();
  }
  m_dirtyChunks.clear();
}

void World::RemeshAllChunks() {
  m_chunks.forEach([&](glm::ivec2 coord, std::shared_ptr<Chunk> chunk) {
    chunk->InvalidateMesh();
    // long distance = GetDistanceToChunk(coord, *this);
    m_chunksToGenerateMesh.push(chunk);
  });
}
