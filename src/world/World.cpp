#include <memory>

#include "World.h"
#include "Chunk.h"
#include "util/MathUtil.h"
#include "util/Logging.h"

World::World() {

  for (int x = -3; x <= 3; x++) {
    for (int z = -3; z <= 3; z++) {
      glm::ivec2 chunkCoord(x, z);
      m_chunks[chunkCoord] = std::make_unique<Chunk>(chunkCoord, *this);
      m_chunks[chunkCoord]->GenerateTerrain();
    }
  }

  for (const auto& [chunkCoord, chunk] : m_chunks) {
    chunk->GenerateMesh();
  }

}

bool World::GetBlockstateAt(int globalX, int globalY, int globalZ) const {
  if (IsInsideWorld(globalX, globalY, globalZ)) {
    glm::ivec2 chunkCoord = GetChunkCoord(globalX, globalZ);
    glm::ivec3 localCoords = ToLocalCoords(globalX, globalY, globalZ);
    bool result = m_chunks.at(chunkCoord)->GetBlockstateAt(localCoords.x, localCoords.y, localCoords.z);
    return result;
  } else {
    return false;
  }
}

void World::Draw(Shader& shader) const {
  for (const auto& [chunkCoord, chunk] : m_chunks) {
    chunk->Draw(shader);
  }
}

bool World::IsInsideWorld(int globalX, int globalY, int globalZ) const {
  if (globalY < 0 || globalY >= Chunk::CHUNK_HEIGHT) {
    return false;
  }
  glm::ivec2 chunkCoord = GetChunkCoord(globalX, globalZ);
  return m_chunks.find(chunkCoord) != m_chunks.end();
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
