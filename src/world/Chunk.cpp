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

#include "../init/Blocks.h"
#include "../block/Block.h"

const int Chunk::SUBCHUNK_HEIGHT = 256;
const int Chunk::SUBCHUNK_LAYERS = 1;
const int Chunk::CHUNK_WIDTH = 16;
const int Chunk::CHUNK_HEIGHT = Chunk::SUBCHUNK_HEIGHT * Chunk::SUBCHUNK_LAYERS;

Chunk::Chunk(glm::ivec2 chunkCoord, World& world)
  : m_neighbors(8, nullptr), m_chunkCoord(chunkCoord), m_blockstates(CHUNK_WIDTH* CHUNK_HEIGHT* CHUNK_WIDTH), m_lights(CHUNK_WIDTH* CHUNK_HEIGHT* CHUNK_WIDTH, 0), m_subchunkMeshes(SUBCHUNK_LAYERS), m_world(world) {}

void Chunk::GenerateMesh() {
  // Generate the mesh for each subchunk

  m_subchunkMeshesData.reserve(SUBCHUNK_LAYERS);

  for (int i = 0; i < SUBCHUNK_LAYERS; i++) {
    GenerateMeshForSubchunk(i);
  }

  m_generatedMesh = true;
}

void Chunk::PropagateLighting() {

  for (int x = 0; x < CHUNK_WIDTH; x++) {
    for (int y = 0; y < CHUNK_HEIGHT; y++) {
      for (int z = 0; z < CHUNK_WIDTH; z++) {
        char light = GetLightAt(x, y, z);
        if (light > 0) {
          LightDFS(x, y, z, light);
        }
      }
    }
  }

  m_propagatedLighting = true;
}

void Chunk::LightDFS(int x, int y, int z, char value) {
  if (value <= 0) return;

  SetLightAt(x, y, z, value);

  static glm::ivec3 spreadDirections[] = { {1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1} };
  for (const glm::ivec3& offset : spreadDirections) {
    int newX = x + offset.x;
    int newY = y + offset.y;
    int newZ = z + offset.z;
    if (newY < 0 || newY >= CHUNK_HEIGHT || GetLightAt(newX, newY, newZ) >= value - 1 || Block::FromBlockstate(GetBlockstateAt(newX, newY, newZ)).IsSolid()) continue;

    LightDFS(newX, newY, newZ, value - 1);
  }
}

int Chunk::GetNeighborIndex(int localX, int localZ) const {
  if (localX < 0 && localZ < 0) {
    return 0;
  } else if (localX >= 0 && localX < CHUNK_WIDTH && localZ < 0) {
    return 1;
  } else if (localX >= CHUNK_WIDTH && localZ < 0) {
    return 2;
  } else if (localX >= CHUNK_WIDTH && localZ >= 0 && localZ < CHUNK_WIDTH) {
    return 3;
  } else if (localX >= CHUNK_WIDTH && localZ >= CHUNK_WIDTH) {
    return 4;
  } else if (localX >= 0 && localX < CHUNK_WIDTH && localZ >= CHUNK_WIDTH) {
    return 5;
  } else if (localX < 0 && localZ >= CHUNK_WIDTH) {
    return 6;
  } else { // localX < 0 && localZ >= 0 && localZ < CHUNK_WIDTH
    return 7;
  }
}

Chunk* Chunk::GetNeighbor(int localX, int localZ) {
  int neighborIndex = GetNeighborIndex(localX, localZ);
  if (m_neighbors[neighborIndex] != nullptr) {
    return m_neighbors[neighborIndex];
  } else {
    glm::ivec3 globalCoords = ToGlobalCoords(localX, 0, localZ);
    Chunk* neighbor = m_world.GetChunkAtBlockPos(globalCoords.x, globalCoords.z);
    m_neighbors[neighborIndex] = neighbor;
    return neighbor;
  }
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

      // (X, Z) calculations
      double noiseValue = Noise::Noise2D(x0 + x, z0 + z, settings.noiseOffsets[0], settings.noiseOffsets[1], settings.noiseScale, settings.octaves, settings.persistence, settings.lacunarity);
      int terrainDifference = MathUtil::FloorToInt(MathUtil::Map(noiseValue, -1, 1, settings.terrainRange[0], settings.terrainRange[1]));
      int terrainHeight = settings.baseTerrainHeight + terrainDifference;

      double treeValue = Noise::RandomNoise2D(x0 + x, z0 + z, 0, 0);
      int treeHeight = MathUtil::IntLerp(5, 9, Noise::RandomNoise2D(x0 + x, z0 + z, 1000, 1000));

      for (int y = 0; y < CHUNK_HEIGHT; y++) {

        char blockstate;

        // TERRAIN PASS
        if (y > terrainHeight) {
          blockstate = Blocks::AIR;
        } else if (y == terrainHeight) {
          blockstate = Blocks::GRASS;
        } else if (y > terrainHeight - 3) {
          blockstate = Blocks::DIRT;
        } else if (y > 0) {
          blockstate = Blocks::STONE;
        } else {
          blockstate = Blocks::BEDROCK;
        }

        // CAVE PASS
        if (blockstate != Blocks::AIR && y >= 2) {
          double caveNoiseValue = (Noise::Noise3D(x0 + x, y, z0 + z, settings.caveNoiseOffsets[0], settings.caveNoiseOffsets[1], settings.caveNoiseOffsets[2], settings.caveNoiseScale) + 1.0) / 2.0;
          if (caveNoiseValue < settings.caveThreshold) {
            blockstate = Blocks::CAVE_AIR;
          }
        }

        // ORE PASS
        if (blockstate == Blocks::STONE) {
          double coalNoiseValue = (Noise::Noise3D(x0 + x, y, z0 + z, 1000, 1000, 1000, settings.coalScale) + 1.0) / 2.0;
          if (coalNoiseValue < settings.coalThreshold) {
            blockstate = Blocks::COAL_ORE;
          }

          double ironNoiseValue = (Noise::Noise3D(x0 + x, y, z0 + z, 2000, 2000, 1000, settings.ironScale) + 1.0) / 2.0;
          if (ironNoiseValue < settings.ironThreshold) {
            blockstate = Blocks::IRON_ORE;
          }
        }

        // TREE PASS
        if (y > terrainHeight && m_blockstates[PosToIndex(x, terrainHeight, z)] == Blocks::GRASS) {
          if (treeValue < 0.01) {
            if (y < terrainHeight + treeHeight + 1) {
              blockstate = Blocks::OAK_LOG;
            } else if (y <= terrainHeight + treeHeight + 2) {
              blockstate = Blocks::OAK_LEAVES;
            }
          }
        }

        m_blockstates[PosToIndex(x, y, z)] = blockstate;
      }
    }
  }

  // Fill sky lights
  for (int x = 0; x < CHUNK_WIDTH; x++) {
    for (int z = 0; z < CHUNK_WIDTH; z++) {
      for (int y = CHUNK_HEIGHT - 1; y >= 0; y--) {
        if (Block::FromBlockstate(m_blockstates[PosToIndex(x, y, z)]).IsSolid()) break;
        m_lights[PosToIndex(x, y, z)] = 15;
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
        const Block& block = Block::FromBlockstate(m_blockstates[PosToIndex(x, y + y0, z)]);

        // Skip air blocks
        if (block.IsAir()) continue;

        for (const auto& face : DirectionUtil::GetAllDirections()) {
          glm::ivec3 offset = VoxelData::GetFaceOffset(face);
          // Skip face if there is a neighbor in that direction
          if (Block::FromBlockstate(GetBlockstateAt(x + offset.x, y + offset.y + y0, z + offset.z)).IsSolid()) continue;

          std::vector<float> faceVertices = VoxelData::GetFaceVertices(x, y, z, *this, face, block);
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
  const DebugSettings& settings = DebugSettings::instance;

  model = glm::translate(model, { m_chunkCoord.x * (CHUNK_WIDTH + settings.chunkSplit), 0, m_chunkCoord.y * (CHUNK_WIDTH + settings.chunkSplit) });
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

char Chunk::GetBlockstateAt(int localX, int localY, int localZ) {
  if (IsInsideChunk(localX, localY, localZ)) {
    return m_blockstates[PosToIndex(localX, localY, localZ)];
  } else if (localY >= 0 && localY < CHUNK_HEIGHT) {
    Chunk* neighbor = GetNeighbor(localX, localZ);
    glm::ivec3 neighborCoords = ToNeighborCoords(localX, localY, localZ);
    return neighbor->GetBlockstateAt(neighborCoords.x, neighborCoords.y, neighborCoords.z);
  }
  return Blocks::VOID_AIR;
}

char Chunk::GetLightAt(int localX, int localY, int localZ) {
  if (IsInsideChunk(localX, localY, localZ)) {
    return m_lights[PosToIndex(localX, localY, localZ)];
  } else if (localY >= 0 && localY < CHUNK_HEIGHT) {
    Chunk* neighbor = GetNeighbor(localX, localZ);
    glm::ivec3 neighborCoords = ToNeighborCoords(localX, localY, localZ);
    return neighbor->GetLightAt(neighborCoords.x, neighborCoords.y, neighborCoords.z);
  }
  return 0;
}

float Chunk::GetFixedLightAt(int localX, int localY, int localZ) {
  if (localY < 0 || localY >= CHUNK_HEIGHT) return 0;
  const Block& block = Block::FromBlockstate(GetBlockstateAt(localX, localY, localZ));
  char lightLevel;
  if (block.IsSolid()) {
    lightLevel = -1;
  } else {
    lightLevel = GetLightAt(localX, localY, localZ);
  }
  float light = lightLevel / 15.0f;
  return light;
}

void Chunk::SetLightAt(int localX, int localY, int localZ, char value) {
  if (IsInsideChunk(localX, localY, localZ)) {
    m_lights[PosToIndex(localX, localY, localZ)] = value;
  } else if (localY >= 0 && localY < CHUNK_HEIGHT) {
    Chunk* neighbor = GetNeighbor(localX, localZ);
    glm::ivec3 neighborCoords = ToNeighborCoords(localX, localY, localZ);
    return neighbor->SetLightAt(neighborCoords.x, neighborCoords.y, neighborCoords.z, value);
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

bool Chunk::HasPropagatedLighting() const {
  return m_propagatedLighting;
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

glm::ivec3 Chunk::ToNeighborCoords(int localX, int localY, int localZ) const {
  return {
    MathUtil::Mod(localX, Chunk::CHUNK_WIDTH),
    localY,
    MathUtil::Mod(localZ, Chunk::CHUNK_WIDTH)
  };
}