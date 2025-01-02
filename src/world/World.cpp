#include <memory>
#include <thread>
#include <functional>

#include "World.h"
#include "Chunk.h"
#include "util/MathUtil.h"
#include "util/Logging.h"

namespace {
const int RENDER_DISTANCE = 12;
const int WORKER_COUNT = 3;
} // namespace

void chunkTerrainGeneratorWorker(World& world, int workerId) {

  // TODO: Remove duplicated from the chunk generation worker
  static glm::ivec2 chunkOffsets[] = { {0, 0}, {0, 1}, {1, 0}, {0, -1}, {-1, 0} };

  while (true) {
    Chunk* chunk;
    if (!world.m_chunksToGenerateTerrain.pop(chunk)) break;

    chunk->GenerateTerrain();

    // Check if a neighboring chunk, or this one, is ready for meshing
    glm::ivec2 centerCoord = chunk->GetChunkCoord();
    for (const glm::ivec2& offset : chunkOffsets) {
      glm::ivec2 chunkCoord = centerCoord + offset;

      if (!world.m_chunks.contains(chunkCoord)) {
        continue;
      }

      // Check all the neighbors
      Chunk* currChunk = world.GetChunkAt(chunkCoord);
      bool ready = true;
      for (glm::ivec2& secondOffset : chunkOffsets) {
        glm::ivec2 secondChunkCoord = chunkCoord + secondOffset;

        if (!world.m_chunks.contains(secondChunkCoord)) {
          ready = false;
          break;
        }

        Chunk* neighborChunk = world.GetChunkAt(secondChunkCoord);
        if (!neighborChunk->HasGeneratedTerrain()) {
          ready = false;
          break;
        }
      }

      if (ready && !currChunk->a_queuedMesh.exchange(true)) {
        world.m_chunksToGenerateMesh.push(currChunk);
      }

    }
  }

  LOG(INFO) << "Chunk terrain generator worker #" << workerId << " stopped";
}

void chunkMeshGeneratorWorker(World& world, int workerId) {
  while (true) {
    Chunk* chunk;
    if (!world.m_chunksToGenerateMesh.pop(chunk)) break;

    chunk->GenerateMesh();
    world.m_chunksToApplyMesh.push(chunk);
  }

  LOG(INFO) << "Chunk mesh generator worker #" << workerId << " stopped";
}

World::World() {}

World::~World() {
  // Stop the worker threads
  m_chunkGenerationQueue.stop();
  m_chunksToGenerateTerrain.stop();
  m_chunksToGenerateMesh.stop();

  m_chunks.forEach([](glm::ivec2 coord, Chunk* chunk) {
    delete chunk;
  });

  for (auto& thread : m_workerThreads) {
    if (thread.joinable()) {
      thread.join();
    }
  }
}

void World::Start() {
  // Generate the worker threads
  auto sharedWorld = std::shared_ptr<World>(this);

  for (int i = 0; i < WORKER_COUNT; i++) {
    m_workerThreads.push_back(std::thread([sharedWorld, i]() {
      chunkTerrainGeneratorWorker(*sharedWorld, i);
    }));

    m_workerThreads.push_back(std::thread([sharedWorld, i]() {
      chunkMeshGeneratorWorker(*sharedWorld, i);
    }));
  }

}

void World::Update(glm::vec3 playerPosition) {

  glm::ivec2 playerChunk = GetChunkCoord((int)floor(playerPosition.x), (int)floor(playerPosition.z));

  m_chunks.forEach([](glm::ivec2 coord, Chunk* chunk) {
    chunk->SetActive(false);
  });

  for (int x = -RENDER_DISTANCE; x <= RENDER_DISTANCE; x++) {
    for (int z = -RENDER_DISTANCE; z <= RENDER_DISTANCE; z++) {
      glm::ivec2 chunkCoord(x, z);
      chunkCoord += playerChunk;

      Chunk* chunk = GetOrCreateChunkAt(chunkCoord);
      chunk->SetActive(true);

      // Queue the chunk for generation if it hasnt been already
      // TODO: a_queuedGeneration does not need to be atomic anymore
      if (!chunk->a_queuedGeneration.exchange(true)) {
        int distance = std::abs(x) + std::abs(z);
        m_chunkGenerationQueue.push({ distance, chunk });
      }
    }
  }

  // Go through the chunks that need to be generated
  while (!m_chunkGenerationQueue.empty()) {
    // Chunks that need to be generated in order to make the mesh for the chunk at 0, 0
    static glm::ivec2 chunkOffsets[] = { {0, 0}, {0, 1}, {1, 0}, {0, -1}, {-1, 0} };
    DistanceToChunk distanceToChunk;
    if (!m_chunkGenerationQueue.pop(distanceToChunk)) break;

    Chunk* centerChunk = distanceToChunk.second;
    glm::ivec2 centerCoord = centerChunk->GetChunkCoord();

    // We need to generate the terrain for this chunk and all other adjacent chunks
    for (const glm::ivec2& offset : chunkOffsets) {
      glm::ivec2 chunkCoord = centerCoord + offset;
      Chunk* chunk = GetOrCreateChunkAt(chunkCoord);

      if (!chunk->a_queuedTerrain.exchange(true)) {
        m_chunksToGenerateTerrain.push(chunk);
      }
    }
  }

  while (!m_chunksToApplyMesh.empty()) {
    Chunk* chunk;
    m_chunksToApplyMesh.pop(chunk);

    chunk->ApplyMesh();
  }
}

int World::GetChunksToGenerateTerrainSize() const {
  return m_chunksToGenerateTerrain.size();
}

int World::GetChunksToGenerateMeshSize() const {
  return m_chunksToGenerateMesh.size();
}

bool World::GetBlockstateAt(int globalX, int globalY, int globalZ) const {
  if (IsInsideWorld(globalX, globalY, globalZ)) {
    glm::ivec2 chunkCoord = GetChunkCoord(globalX, globalZ);
    glm::ivec3 localCoords = ToLocalCoords(globalX, globalY, globalZ);
    bool result = GetChunkAt(chunkCoord)->GetBlockstateAt(localCoords.x, localCoords.y, localCoords.z);
    return result;
  } else {
    return false;
  }
}

void World::Draw(Shader& shader) const {
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
  Chunk* chunk = m_chunks.get(chunkCoord).value();
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