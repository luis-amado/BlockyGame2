#include "Chunk.h"

#include <glm/gtc/matrix_transform.hpp>

#include "util/Logging.h"
#include "rendering/Shader.h"
#include "../voxel/Direction.h"
#include "../voxel/VoxelData.h"

const int Chunk::SUBCHUNK_HEIGHT = 16;
const int Chunk::SUBCHUNK_LAYERS = 16;
const int Chunk::CHUNK_WIDTH = 16;
const int Chunk::CHUNK_HEIGHT = SUBCHUNK_HEIGHT * SUBCHUNK_LAYERS;

Chunk::Chunk(glm::ivec2 chunkCoord)
  : m_chunkCoord(chunkCoord), m_blockstates(CHUNK_WIDTH* CHUNK_HEIGHT* CHUNK_WIDTH), m_subchunkMeshes(SUBCHUNK_LAYERS) {

  GenerateTerrain();

  // Generate the mesh for each subchunk
  for (int i = 0; i < SUBCHUNK_LAYERS; i++) {
    GenerateMeshForSubchunk(i);
  }
}

void Chunk::GenerateTerrain() {
  // For now, I'm filling up the whole chunk with solid blocks

  int terrainHeight = 60;

  for (int x = 0; x < CHUNK_WIDTH; x++) {
    for (int z = 0; z < CHUNK_WIDTH; z++) {
      for (int y = 0; y < CHUNK_HEIGHT; y++) {
        if (y <= terrainHeight) {
          m_blockstates[PosToIndex(x, y, z)] = true;
        } else {
          m_blockstates[PosToIndex(x, y, z)] = false;
        }
      }
    }
  }
}

void Chunk::GenerateMeshForSubchunk(int i) {
  // Need to generate a list of vertices and indices

  std::vector<float> vertices;
  std::vector<unsigned int> indices;

  int vertexCount = 0;

  int y0 = i * SUBCHUNK_HEIGHT;

  for (int x = 0; x < CHUNK_WIDTH; x++) {
    for (int y = 0; y < SUBCHUNK_HEIGHT; y++) {
      for (int z = 0; z < CHUNK_WIDTH; z++) {
        // Skip empty blocks
        if (!m_blockstates[PosToIndex(x, y + y0, z)]) continue;

        for (const auto& face : DirectionUtil::GetAllDirections()) {
          glm::ivec3 offset = VoxelData::GetFaceOffset(face);
          // Skip face if there is a neighbor in that direction
          if (GetBlockstateAt(x + offset.x, y + offset.y + y0, z + offset.z)) continue;

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

  m_subchunkMeshes[i].SetData(vertices.data(), vertices.size(), indices.data(), indices.size());
}

void Chunk::Draw(Shader& shader) const {
  glm::mat4 model(1.0f);
  model = glm::translate(model, { m_chunkCoord.x * CHUNK_WIDTH, 0, m_chunkCoord.y * CHUNK_WIDTH });
  for (int i = 0; i < SUBCHUNK_LAYERS; i++) {
    if (m_subchunkMeshes[i].HasData()) {
      shader.LoadMatrix4f("model", model);
      m_subchunkMeshes[i].Draw();
    } else {
      LOG(WARN) << "Tried to draw subchunk at (" << m_chunkCoord.x << ", " << i << ", " << m_chunkCoord.y << ") before generating mesh";
    }

    // Translate the model upwards
    model = glm::translate(model, { 0, SUBCHUNK_HEIGHT, 0 });
  }
}

bool Chunk::GetBlockstateAt(int x, int y, int z) const {
  if (IsInsideChunk(x, y, z)) {
    return m_blockstates[PosToIndex(x, y, z)];
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
