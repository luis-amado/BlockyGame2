#include "Chunk.h"

#include <glm/gtc/matrix_transform.hpp>

#include "util/Logging.h"
#include "util/Noise.h"
#include "util/MathUtil.h"
#include "rendering/Shader.h"
#include "../voxel/Direction.h"
#include "../voxel/VoxelData.h"
#include "World.h"
#include "../debug/DebugSettings.h"

const int Chunk::SUBCHUNK_HEIGHT = 256;
const int Chunk::SUBCHUNK_LAYERS = 1;
const int Chunk::CHUNK_WIDTH = 16;
const int Chunk::CHUNK_HEIGHT = Chunk::SUBCHUNK_HEIGHT * Chunk::SUBCHUNK_LAYERS;

Chunk::Chunk(glm::ivec2 chunkCoord, World& world)
  : m_chunkCoord(chunkCoord), m_blockstates(CHUNK_WIDTH* CHUNK_HEIGHT* CHUNK_WIDTH), m_subchunkMeshes(SUBCHUNK_LAYERS), m_world(world) {}

void Chunk::GenerateMesh() {
  // Generate the mesh for each subchunk

  m_subchunkMeshesData.reserve(SUBCHUNK_LAYERS);

  for (int i = 0; i < SUBCHUNK_LAYERS; i++) {
    GenerateMeshForSubchunk(i);
  }

  m_generatedMesh = true;
}

void Chunk::ApplyMesh() {
  for (int i = 0; i < SUBCHUNK_LAYERS; i++) {
    std::vector<float>& vertices = m_subchunkMeshesData[i].vertices;
    std::vector<unsigned int>& indices = m_subchunkMeshesData[i].indices;

    m_subchunkMeshes[i].SetData(vertices.data(), vertices.size(), indices.data(), indices.size());
  }

  m_subchunkMeshesData.clear();

  m_appliedMesh = true;
}

void Chunk::GenerateTerrain() {

  int x0 = m_chunkCoord.x * CHUNK_WIDTH;
  int z0 = m_chunkCoord.y * CHUNK_WIDTH;

  const DebugSettings& settings = DebugSettings::instance;

  for (int x = 0; x < CHUNK_WIDTH; x++) {
    for (int z = 0; z < CHUNK_WIDTH; z++) {
      double noiseValue = Noise::Noise2D(x0 + x, z0 + z, settings.noiseOffsets[0], settings.noiseOffsets[1], settings.noiseScale, settings.octaves, settings.persistence, settings.lacunarity);
      int terrainDifference = MathUtil::FloorToInt(MathUtil::Map(noiseValue, -1, 1, settings.terrainRange[0], settings.terrainRange[1]));
      int terrainHeight = settings.baseTerrainHeight + terrainDifference;
      for (int y = 0; y < CHUNK_HEIGHT; y++) {
        if (y <= terrainHeight) {
          m_blockstates[PosToIndex(x, y, z)] = true;
        } else {
          m_blockstates[PosToIndex(x, y, z)] = false;
        }
      }
    }
  }

  m_generatedTerrain = true;
}

void Chunk::GenerateMeshForSubchunk(int i) {
  // Need to generate a list of vertices and indices

  std::vector<float>& vertices = m_subchunkMeshesData[i].vertices;
  std::vector<unsigned int>& indices = m_subchunkMeshesData[i].indices;

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

}

void Chunk::Draw(Shader& shader) const {
  // TODO: Use some sort of frustum culling to prevent non-visible chunks from being drawn
  if (!m_active || !m_appliedMesh) return;

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

bool Chunk::GetBlockstateAt(int localX, int localY, int localZ) const {
  if (IsInsideChunk(localX, localY, localZ)) {
    return m_blockstates[PosToIndex(localX, localY, localZ)];
  } else {
    glm::ivec3 globalCoords = ToGlobalCoords(localX, localY, localZ);
    return m_world.GetBlockstateAt(globalCoords.x, globalCoords.y, globalCoords.z);
  }
}

void Chunk::SetActive(bool value) {
  m_active = value;
}

bool Chunk::HasAppliedMesh() const {
  return m_appliedMesh;
}

bool Chunk::HasGeneratedTerrain() const {
  return m_generatedTerrain;
}

bool Chunk::HasGeneratedMesh() const {
  return m_generatedMesh;
}

glm::ivec2 Chunk::GetChunkCoord() const {
  return m_chunkCoord;
}

inline int Chunk::PosToIndex(int localX, int localY, int localZ) const {
  return localX + localZ * CHUNK_WIDTH + localY * CHUNK_WIDTH * CHUNK_WIDTH;
}

bool Chunk::IsInsideChunk(int localX, int localY, int localZ) const {
  return localX >= 0 && localY >= 0 && localZ >= 0 && localX < CHUNK_WIDTH && localY < CHUNK_HEIGHT && localZ < CHUNK_WIDTH;
}

glm::ivec3 Chunk::ToGlobalCoords(int localX, int localY, int localZ) const {
  return {
    localX + CHUNK_WIDTH * m_chunkCoord.x,
    localY,
    localZ + CHUNK_WIDTH * m_chunkCoord.y
  };
}
