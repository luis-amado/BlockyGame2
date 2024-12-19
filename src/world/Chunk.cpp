#include "Chunk.h"

#include <glm/gtc/matrix_transform.hpp>

#include "util/Logging.h"
#include "rendering/Shader.h"
#include "../voxel/Direction.h"
#include "../voxel/VoxelData.h"

const int Chunk::CHUNK_WIDTH = 16;
const int Chunk::CHUNK_HEIGHT = 16;

Chunk::Chunk(glm::ivec2 chunkCoord)
  : m_chunkCoord(chunkCoord), blockstates(CHUNK_WIDTH* CHUNK_HEIGHT* CHUNK_WIDTH) {

  GenerateTerrain();
  GenerateMesh();
}

void Chunk::GenerateTerrain() {
  // For now, I'm filling up the whole chunk with solid blocks
  for (int x = 0; x < CHUNK_WIDTH; x++) {
    for (int z = 0; z < CHUNK_WIDTH; z++) {
      for (int y = 0; y < CHUNK_WIDTH; y++) {
        if (rand() % 10 > 7) continue;
        blockstates[PosToIndex(x, y, z)] = true;
      }
    }
  }
}

void Chunk::GenerateMesh() {
  // Need to generate a list of vertices and indices

  std::vector<float> vertices;
  std::vector<unsigned int> indices;

  int vertexCount = 0;

  for (int x = 0; x < CHUNK_WIDTH; x++) {
    for (int y = 0; y < CHUNK_HEIGHT; y++) {
      for (int z = 0; z < CHUNK_WIDTH; z++) {
        // Skip empty blocks
        if (!blockstates[PosToIndex(x, y, z)]) continue;

        for (const auto& face : DirectionUtil::GetAllDirections()) {
          glm::ivec3 offset = VoxelData::GetFaceOffset(face);
          // Skip face if there is a neighbor in that direction
          if (GetBlockstateAt(x + offset.x, y + offset.y, z + offset.z)) continue;

          std::vector<float> faceVertices = VoxelData::GetFaceVertices(x, y, z, face);
          vertices.insert(vertices.end(), faceVertices.begin(), faceVertices.end());

          // Indices: 0, 1, 2, 2, 3, 0
          indices.push_back(vertexCount + 0);
          indices.push_back(vertexCount + 1);
          indices.push_back(vertexCount + 2);
          indices.push_back(vertexCount + 2);
          indices.push_back(vertexCount + 3);
          indices.push_back(vertexCount + 0);
          vertexCount += 4;
        }

      }
    }
  }

  m_mesh = Mesh(vertices.data(), vertices.size(), indices.data(), indices.size());
}

void Chunk::Draw(Shader& shader) const {
  if (m_mesh.has_value()) {

    glm::mat4 model(1.0f);
    model = glm::translate(model, { m_chunkCoord.x * CHUNK_WIDTH, 0, m_chunkCoord.y * CHUNK_WIDTH });
    shader.LoadMatrix4f("model", model);

    m_mesh->Draw();
  } else {
    LOG(WARN) << "Tried to draw chunk at (" << m_chunkCoord.x << ", " << m_chunkCoord.y << ") before generating mesh";
  }
}

bool Chunk::GetBlockstateAt(int x, int y, int z) const {
  if (IsInsideChunk(x, y, z)) {
    return blockstates[PosToIndex(x, y, z)];
  } else {
    return false;
  }
}

inline int Chunk::PosToIndex(int x, int y, int z) const {
  return x + z * CHUNK_WIDTH + y * CHUNK_WIDTH * CHUNK_WIDTH;
}

bool Chunk::IsInsideChunk(int x, int y, int z) const {
  return x >= 0 && y >= 0 && z >= 0 && x < CHUNK_WIDTH && y < CHUNK_HEIGHT && z < CHUNK_WIDTH;
}
