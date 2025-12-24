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
#include "../voxel/VoxelData.h"
#include "util/threadsafe/ThreadSafeReference.h"

#include "../debug/DebugSettings.h"
#include "../init/Blocks.h"

namespace {
glm::ivec2 chunkOffsetsWithCorners[] = { {0, 0}, {0, 1}, {1, 0}, {0, -1}, {-1, 0}, {-1, -1}, {1, -1}, {1, 1}, {-1, 1} };
} // namespace

void chunkTerrainGeneratorWorker(World& world, int workerId) {

  while (true) {
    glm::ivec2 grabbedChunkCoord;
    if (!world.m_chunksToGenerateTerrain.pop(grabbedChunkCoord)) break;

    ThreadSafeReference<Chunk> chunk = world.GetChunkRefAt(grabbedChunkCoord);
    if (chunk.Get() == nullptr) continue;

    if (!chunk->HasGeneratedTerrain()) {
      chunk->GenerateTerrain();
    }

    // Check if a neighboring chunk, or this one, is ready for lighting
    glm::ivec2 centerCoord = chunk->GetChunkCoord();
    for (const glm::ivec2& offset : chunkOffsetsWithCorners) {
      glm::ivec2 chunkCoord = centerCoord + offset;

      if (!world.m_chunks.contains(chunkCoord)) {
        continue;
      }

      // Check all the neighbors
      ThreadSafeReference<Chunk> currChunk = world.GetChunkRefAt(chunkCoord);
      if (currChunk.Get() == nullptr) continue;

      bool ready = true;
      for (glm::ivec2& secondOffset : chunkOffsetsWithCorners) {
        glm::ivec2 secondChunkCoord = chunkCoord + secondOffset;

        if (!world.m_chunks.contains(secondChunkCoord)) {
          ready = false;
          break;
        }

        ThreadSafeReference<Chunk> neighborChunk = world.GetChunkRefAt(secondChunkCoord);
        if (neighborChunk.Get() == nullptr) continue;

        if (!neighborChunk->HasGeneratedTerrain()) {
          ready = false;
          break;
        }
      }

      if (ready && !currChunk->a_queuedLighting.exchange(true)) {
        world.m_chunksToPropagateLighting.push(currChunk->GetChunkCoord());
      }

    }

  }

  LOG(EXTRA) << "Chunk terrain generator worker #" << workerId << " stopped";
}

void chunkLightingWorker(World& world, int workerId) {
  while (true) {
    glm::ivec2 grabbedChunkCoord;
    if (!world.m_chunksToPropagateLighting.pop(grabbedChunkCoord)) break;

    ThreadSafeReference<Chunk> chunk = world.GetChunkRefAt(grabbedChunkCoord);
    if (chunk.Get() == nullptr) continue;

    if (!chunk->HasPropagatedLighting()) {
      chunk->PropagateLighting();
    }

    // Check if a neighboring chunk, or this one, is ready for meshing
    glm::ivec2 centerCoord = chunk->GetChunkCoord();
    for (const glm::ivec2& offset : chunkOffsetsWithCorners) {
      glm::ivec2 chunkCoord = centerCoord + offset;

      if (!world.m_chunks.contains(chunkCoord)) {
        continue;
      }

      ThreadSafeReference<Chunk> currChunk = world.GetChunkRefAt(chunkCoord);
      if (currChunk.Get() == nullptr || currChunk->a_queuedMesh) continue;

      // Check all the neighbors
      bool ready = true;
      for (glm::ivec2& secondOffset : chunkOffsetsWithCorners) {
        glm::ivec2 secondChunkCoord = chunkCoord + secondOffset;

        if (!world.m_chunks.contains(secondChunkCoord)) {
          ready = false;
          break;
        }

        ThreadSafeReference<Chunk> neighborChunk = world.GetChunkRefAt(secondChunkCoord);
        if (neighborChunk.Get() == nullptr) continue;

        if (!neighborChunk->HasPropagatedLighting()) {
          ready = false;
          break;
        }
      }

      if (ready && !currChunk->a_queuedMesh.exchange(true)) {
        world.m_chunksToGenerateMesh.push(currChunk->GetChunkCoord());
      }

    }
  }

  LOG(EXTRA) << "Chunk lighting worker #" << workerId << " stopped";
}

void chunkMeshGeneratorWorker(World& world, int workerId) {
  while (true) {
    glm::ivec2 grabbedChunkCoord;
    if (!world.m_chunksToGenerateMesh.pop(grabbedChunkCoord)) break;

    ThreadSafeReference<Chunk> chunk = world.GetChunkRefAt(grabbedChunkCoord);
    if (chunk.Get() == nullptr) continue;

    glm::vec3 cameraPosition = world.m_trackingEntity.GetPosition();
    glm::ivec2 cameraChunkCoord = world.GetChunkCoord(MathUtil::FloorToInt(cameraPosition.x), MathUtil::FloorToInt(cameraPosition.z));
    glm::ivec2 chunkCoord = chunk->GetChunkCoord();

    // Check if chunk is too far away and if so, skip generating the mesh and instead generate it later if it's needed
    if (std::abs(chunkCoord.x - cameraChunkCoord.x) + std::abs(chunkCoord.y - cameraChunkCoord.y) > DebugSettings::instance.renderDistance * 2) {
      chunk->a_queuedTerrain = false;
      chunk->a_queuedLighting = false;
      chunk->a_queuedMesh = false;
      chunk->m_queuedGeneration = false;
      continue;
    }

    chunk->GenerateMesh();
    world.m_chunksToApplyMesh.push(chunk->GetChunkCoord());
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

  // Delete all chunks
  m_chunks.forEach([](glm::ivec2 coord, Chunk* chunk) {
    delete chunk;
  });
  m_chunks.clear();
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

  std::vector<glm::ivec2> chunksToUnload;

  m_chunks.forEach([&](glm::ivec2 coord, Chunk* chunk) {
    chunk->SetActive(false);

    if (std::abs(coord.x - playerChunk.x) > inMemoryRadius || std::abs(coord.y - playerChunk.y) > inMemoryRadius) {
      chunksToUnload.push_back(coord);
    }
  });

  for (const glm::ivec2& coord : chunksToUnload) {
    Chunk* chunk = m_chunks.get(coord).value_or(nullptr);
    m_chunks.erase(coord);
    chunk->MarkToUnload();
  }

  int renderDistance2 = renderDistance * renderDistance;

  for (int x = -renderDistance; x <= renderDistance; x++) {
    for (int z = -renderDistance; z <= renderDistance; z++) {

      // Filter out chunks that are outside the render distance "circle"
      if (x * x + z * z >= renderDistance2) {
        continue;
      }

      glm::ivec2 chunkCoord(x, z);
      chunkCoord += playerChunk;

      Chunk* chunk = GetOrCreateChunkAt(chunkCoord);
      chunk->SetActive(true);

      // Queue the chunk for generation if it hasnt been already
      if (!chunk->m_queuedGeneration) {
        int distance = std::abs(x) + std::abs(z);
        m_chunkGenerationQueue.push({ distance, chunk });
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
      Chunk* chunk = GetOrCreateChunkAt(chunkCoord);

      if (!chunk->a_queuedTerrain.exchange(true)) {
        m_chunksToGenerateTerrain.push(chunk->GetChunkCoord());
      }
    }
  }

  while (!m_chunksToApplyMesh.empty()) {
    glm::ivec2 chunkCoord;
    m_chunksToApplyMesh.pop(chunkCoord);

    Chunk* chunk = GetChunkAt(chunkCoord);
    if (chunk == nullptr) continue;

    chunk->ApplyMesh();
  }
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

void World::MarkChunkDirty(Chunk* chunk) {
  m_dirtyChunks.insert(chunk);
}

void World::MarkPositionDirty(const glm::ivec3& globalPosition) const {
  Chunk* chunk = GetChunkAtBlockPos(globalPosition.x, globalPosition.z);
  chunk->MarkPositionDirty(ToLocalCoords(globalPosition));
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
  m_chunks.forEach([&](glm::ivec2 coord, Chunk* chunk) {
    chunk->Draw(shader);
  });
}

Chunk* World::GetOrCreateChunkAt(glm::ivec2 chunkCoord) {
  Chunk* chunk;

  // Create the chunk if it doesnt exist
  if (!m_chunks.contains(chunkCoord)) {
    chunk = new Chunk(chunkCoord, *this);
    m_chunks.insert(chunkCoord, chunk);
  } else {
    chunk = m_chunks.get(chunkCoord).value();
  }

  return chunk;
}

Chunk* World::GetChunkAt(glm::ivec2 chunkCoord) const {
  Chunk* chunk = m_chunks.get(chunkCoord).value_or(nullptr);
  return chunk;
}

ThreadSafeReference<Chunk> World::GetChunkRefAt(glm::ivec2 chunkCoord) const {
  Chunk* chunk = m_chunks.get(chunkCoord).value_or(nullptr);
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

void World::UpdateBlockstateAt(int globalX, int globalY, int globalZ, char blockstate) {

  glm::ivec3 position = { globalX, globalY, globalZ };
  std::unordered_set<Chunk*> dirtyChunks;

  Chunk* chunk = GetChunkAtBlockPos(globalX, globalZ);
  glm::ivec3 localCoords = ToLocalCoords(globalX, globalY, globalZ);
  chunk->SetBlockstateAt(localCoords.x, localCoords.y, localCoords.z, blockstate);

  // Mark block and neighbors as dirty (because they may need to adjust their meshes)
  chunk->MarkPositionAndNeighborsDirty(localCoords);

  // Recalculate lighting
  chunk->PropagateLightingAtPos(localCoords, 0);

  CleanDirtyChunks();
}

Chunk* World::GetChunkAtBlockPos(int globalX, int globalZ) const {
  return m_chunks.get(GetChunkCoord(globalX, globalZ)).value_or(nullptr);
}

ThreadSafeReference<Chunk> World::GetChunkRefAtBlockPos(int globalX, int globalZ) const {
  return m_chunks.get(GetChunkCoord(globalX, globalZ)).value_or(nullptr);
}

void World::CleanDirtyChunks() {
  for (Chunk* chunk : m_dirtyChunks) {
    chunk->CleanDirty();
  }
  m_dirtyChunks.clear();
}
