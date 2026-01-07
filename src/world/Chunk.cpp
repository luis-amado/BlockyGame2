#include "Chunk.h"

#include <glm/gtc/matrix_transform.hpp>
#include <unordered_set>

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

const int Chunk::SUBCHUNK_HEIGHT = 16;
const int Chunk::SUBCHUNK_LAYERS = 16;
const int Chunk::CHUNK_WIDTH = 16;
const int Chunk::CHUNK_HEIGHT = Chunk::SUBCHUNK_HEIGHT * Chunk::SUBCHUNK_LAYERS;

Chunk::Chunk(glm::ivec2 chunkCoord, World& world)
  : m_neighbors(8), m_chunkCoord(chunkCoord), m_blockstates(CHUNK_WIDTH* CHUNK_HEIGHT* CHUNK_WIDTH), m_lights(CHUNK_WIDTH* CHUNK_HEIGHT* CHUNK_WIDTH, { 0, 0 }), m_subchunkMeshes(SUBCHUNK_LAYERS), m_world(world) {}


void Chunk::GenerateMesh() {
  // Generate the mesh for each subchunk

  m_subchunkMeshesData.resize(SUBCHUNK_LAYERS);

  for (int i = 0; i < SUBCHUNK_LAYERS; i++) {
    GenerateMeshForSubchunk(i);
  }

  {
    std::lock_guard<std::mutex> lock(m_stateMutex);
    m_generatedMesh = true;
  }
}

void Chunk::PropagateLighting() {
  {
    std::lock_guard<std::mutex> lock(m_stateMutex);
    m_propagatedLighting = true;
  }
  return;

  for (int x = 0; x < CHUNK_WIDTH; x++) {
    for (int y = 0; y < CHUNK_HEIGHT; y++) {
      for (int z = 0; z < CHUNK_WIDTH; z++) {
        char skyLight = GetLightAt(LightType::SKY, x, y, z);
        if (skyLight > 0) LightSpreadingDFS(LightType::SKY, x, y, z, skyLight);

        char blockLight = GetLightAt(LightType::BLOCK, x, y, z);
        if (blockLight > 0) LightSpreadingDFS(LightType::BLOCK, x, y, z, blockLight);
      }
    }
  }

}

void Chunk::PropagateLightingAtPos(glm::ivec3 localPosition, Blockstate oldBlockstate, Blockstate newBlockstate) {
  char oldSkyLight = GetLightAt(LightType::SKY, XYZ(localPosition));
  char oldBlockLight = GetLightAt(LightType::BLOCK, XYZ(localPosition));

  const Block& oldBlock = Block::FromBlockstate(oldBlockstate);
  const Block& newBlock = Block::FromBlockstate(newBlockstate);

  // Handle Sky light
  SetLightAt(LightType::SKY, XYZ(localPosition), 0);
  if (newBlock.IsSolid() && !oldBlock.IsSolid()) {
    // Light removal dfs
    RemoveLight(LightType::SKY, XYZ(localPosition), 0, oldSkyLight);
  } else if (!newBlock.IsSolid() && oldBlock.IsSolid()) {
    LightUpdatingDFS(LightType::SKY, XYZ(localPosition));
  }

  // Handle block light
  char newBlockLightSource = newBlock.GetLightLevel();
  char oldBlockLightSource = oldBlock.GetLightLevel();

  if (newBlockLightSource > oldBlockLight) {
    // Placed a source larger than the current block light: spread
    LightSpreadingDFS(LightType::BLOCK, XYZ(localPosition), newBlockLightSource, /*markDirty=*/true);
  } else if (!newBlock.IsSolid() && oldBlock.IsSolid()) {
    // Unblocked a path light could go through: update
    LightUpdatingDFS(LightType::BLOCK, XYZ(localPosition));
  } else if (newBlock.IsSolid() && !oldBlock.IsSolid()) {
    // Blocked a path light could have been using: remove
    RemoveLight(LightType::BLOCK, XYZ(localPosition), newBlockLightSource, oldBlockLight);
  } else if (newBlockLightSource < oldBlockLight && oldBlockLight == oldBlockLightSource) {
    // Replaced a source with a dimmer source, or removed completely: remove
    RemoveLight(LightType::BLOCK, XYZ(localPosition), newBlockLightSource, oldBlockLight);
  }
}

void Chunk::FillSkyLight() {
  for (int x = 0; x < CHUNK_WIDTH; x++) {
    for (int z = 0; z < CHUNK_WIDTH; z++) {
      for (int y = CHUNK_HEIGHT - 1; y >= 0; y--) {
        if (Block::FromBlockstate(m_blockstates[PosToIndex(x, y, z)]).IsSolid()) break;
        m_lights[PosToIndex(x, y, z)].SetLight(LightType::SKY, 15);
      }
    }
  }
}

// This DFS will spread light values to adjacent blocks
void Chunk::LightSpreadingDFS(LightType type, int x, int y, int z, char value, bool markDirty) {
  if (value <= 0) return;

  // Delegate to other chunk
  if (!IsInsideChunk(x, y, z)) {
    std::shared_ptr<Chunk> neighbor = GetNeighbor(x, z);
    if (neighbor) {
      glm::ivec3 neighborCoords = ToNeighborCoords(x, y, z);
      neighbor->LightSpreadingDFS(type, neighborCoords.x, neighborCoords.y, neighborCoords.z, value, markDirty);
    }
    return;
  }

  SetLightAt(type, x, y, z, value);
  if (markDirty) MarkPositionAndAllNeighborsDirty({ x, y, z });

  static glm::ivec3 spreadDirections[] = { {1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1} };
  for (const glm::ivec3& offset : spreadDirections) {
    int newX = x + offset.x;
    int newY = y + offset.y;
    int newZ = z + offset.z;
    if (newY < 0 || newY >= CHUNK_HEIGHT || GetLightAt(type, newX, newY, newZ) >= value - 1 || Block::FromBlockstate(GetBlockstateAt(newX, newY, newZ)).IsSolid()) continue;

    LightSpreadingDFS(type, newX, newY, newZ, value - 1, markDirty);
  }
}

// This DFS will spread light values to adjacent blocks and also spread values FROM adjacent blocks into the current block
void Chunk::LightUpdatingDFS(LightType type, int x, int y, int z) {

  // Delegate to other chunk
  if (!IsInsideChunk(x, y, z)) {
    std::shared_ptr<Chunk> neighbor = GetNeighbor(x, z);
    if (neighbor) {
      glm::ivec3 neighborCoords = ToNeighborCoords(x, y, z);
      neighbor->LightUpdatingDFS(type, XYZ(neighborCoords));
    }
    return;
  }

  if (Block::FromBlockstate(GetBlockstateAt(x, y, z)).IsSolid() || y < 0 || y >= CHUNK_HEIGHT) {
    return;
  }

  char startingLight = GetLightAt(type, x, y, z);
  char maxLight = startingLight;
  static glm::ivec3 spreadDirections[] = { {1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1} };

  if (GetLightAt(type, x, y + 1, z) == 15) {
    maxLight = 15; // sky light spreads directly downward
  } else {

    for (const glm::ivec3& offset : spreadDirections) {
      int newX = x + offset.x;
      int newY = y + offset.y;
      int newZ = z + offset.z;

      char currLight = GetLightAt(type, newX, newY, newZ);

      if (currLight - 1 > maxLight) {
        maxLight = currLight - 1;
      }
    }

  }

  // spread the light change
  if (maxLight != startingLight) {
    SetLightAt(type, x, y, z, maxLight);
    MarkPositionAndAllNeighborsDirty({ x, y, z });
    for (const glm::ivec3& offset : spreadDirections) {
      int newX = x + offset.x;
      int newY = y + offset.y;
      int newZ = z + offset.z;

      char currLight = GetLightAt(type, newX, newY, newZ);
      if (currLight < maxLight - 1 || (maxLight == 15 && offset.y < 0 && currLight < 15)) {
        LightUpdatingDFS(type, newX, newY, newZ);
      }
    }
  }

}

void Chunk::LightRemovingDFS(LightType type, int x, int y, int z, char value, char oldValue, PositionsToSpreadLightMap& positionsToSpread) {
  if (oldValue <= 0) return;

  // Delegate to proper chunk if necessary
  if (!IsInsideChunk(x, y, z)) {
    std::shared_ptr<Chunk> neighbor = GetNeighbor(x, z);
    if (neighbor) {
      glm::ivec3 neighborCoords = ToNeighborCoords(x, y, z);
      neighbor->LightRemovingDFS(type, XYZ(neighborCoords), value, oldValue, positionsToSpread);
    }
    return;
  }

  SetLightAt(type, x, y, z, value);
  MarkPositionAndAllNeighborsDirty({ x, y, z });

  if (type == LightType::BLOCK && GetBlockAt(x, y, z).GetLightLevel() > value) {
    positionsToSpread[{x, y, z}] = GetBlockAt(x, y, z).GetLightLevel();
  }

  static glm::ivec3 spreadDirections[] = { {1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1} };

  for (const glm::ivec3& offset : spreadDirections) {
    int nX = x + offset.x;
    int nY = y + offset.x;
    int nZ = z + offset.x;

    char neighborLight = GetLightAt(type, nX, nY, nZ);

    bool skyLightOverride = type == LightType::SKY && offset.y < 0 && neighborLight == 15;

    if (neighborLight >= oldValue && !skyLightOverride) {
      positionsToSpread[{nX, nY, nZ}] = neighborLight;
    }

    if (GetBlockAt(nX, nY, nZ).IsSolid()) return;

    char newNeighborValue = std::max(0, value - 1);
    if (neighborLight == oldValue - 1) {
      LightRemovingDFS(type, nX, nY, nZ, newNeighborValue, neighborLight, positionsToSpread);
    } else if (skyLightOverride) {
      LightRemovingDFS(type, nX, nY, nZ, newNeighborValue, oldValue, positionsToSpread);
    }

  }

}

// TODO: This doesn't work
// void Chunk::LightRemovingDFS(LightType type, int x, int y, int z, char value, char startingValue) {
//   if (startingValue <= 0) return;

//   // Delegate to other chunk
//   if (!IsInsideChunk(x, y, z)) {
//     std::shared_ptr<Chunk> neighbor = GetNeighbor(x, z);
//     if (neighbor) {
//       glm::ivec3 neighborCoords = ToNeighborCoords(x, y, z);
//       neighbor->LightRemovingDFS(type, XYZ(neighborCoords), value, startingValue);
//     }
//     return;
//   }

//   // Don't remove light if it's a block light source
//   if (type == LightType::BLOCK && GetLightAt(type, x, y, z) == Block::FromBlockstate(GetBlockstateAt(x, y, z)).GetLightLevel()) return;

//   SetLightAt(type, x, y, z, value);
//   MarkPositionAndAllNeighborsDirty({ x, y, z });

//   static glm::ivec3 spreadDirections[] = { {1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1} };

//   for (const glm::ivec3& offset : spreadDirections) {
//     glm::ivec3 pos = glm::ivec3 { x, y, z } + offset;

//     if (Block::FromBlockstate(GetBlockstateAt(XYZ(pos))).IsSolid()) continue;

//     // Go towards light decreases to block them
//     char currLight = GetLightAt(type, XYZ(pos));
//     char newValue = std::max(0, value - 1);

//     for (const glm::ivec3& offset2 : spreadDirections) {
//       glm::ivec3 neighborPos = pos + offset2;
//       if (Block::FromBlockstate(GetBlockstateAt(XYZ(pos))).IsSolid()) continue;

//       char lightAtBlock = GetLightAt(type, XYZ(neighborPos));
//       // Special check because skylight doesn't travel upwards
//       if (type == LightType::SKY && lightAtBlock - 1 > newValue && (offset2.y >= 0 || lightAtBlock != 15)) {
//         newValue = lightAtBlock - 1;
//       }
//     }

//     if (currLight < startingValue && currLight > newValue) {
//       LightRemovingDFS(type, XYZ(pos), newValue, startingValue - 1);
//     }
//     // Sky light going down
//     if (type == LightType::SKY && startingValue == 15 && currLight == 15 && offset.y < 0) {
//       LightRemovingDFS(type, XYZ(pos), newValue, startingValue);
//     }
//   }

// }

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

std::shared_ptr<Chunk> Chunk::GetNeighbor(int localX, int localZ) {
  int neighborIndex = GetNeighborIndex(localX, localZ);

  if (auto storedNeighbor = m_neighbors[neighborIndex].lock()) {
    return storedNeighbor;
  }

  glm::ivec3 globalCoords = ToGlobalCoords(localX, 0, localZ);
  std::shared_ptr<Chunk> neighbor = m_world.GetChunkAtBlockPos(globalCoords.x, globalCoords.z);

  if (neighbor) {
    m_neighbors[neighborIndex] = neighbor;
  }

  return neighbor;
}

void Chunk::ApplyMesh() {
  for (int i = 0; i < SUBCHUNK_LAYERS; i++) {
    std::vector<float>& vertices = m_subchunkMeshesData[i].vertices;
    std::vector<unsigned int>& indices = m_subchunkMeshesData[i].indices;

    m_subchunkMeshes[i].SetData(vertices.data(), vertices.size(), indices.data(), indices.size());
  }

  m_subchunkMeshesData.resize(Chunk::SUBCHUNK_LAYERS, {});
  {
    std::lock_guard<std::mutex> lock(m_stateMutex);
    m_appliedMesh = true;
  }
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

  FillSkyLight();

  {
    std::lock_guard<std::mutex> lock(m_stateMutex);
    m_generatedTerrain = true;
  }
}

void Chunk::GenerateMeshForSubchunk(int i) {
  // Need to generate a list of vertices and indices

  std::vector<float>& vertices = m_subchunkMeshesData[i].vertices;
  std::vector<unsigned int>& indices = m_subchunkMeshesData[i].indices;

  int vertexCount = 0;

  int y0 = i * SUBCHUNK_HEIGHT;

  vertices.clear();
  indices.clear();

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

          std::vector<float> faceVertices = VoxelData::GetFaceVertices(x, y + y0, z, *this, face, block);
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

void Chunk::RemoveLight(LightType type, int x, int y, int z, char newValue, char oldValue) {
  PositionsToSpreadLightMap positionsToSpread;
  LightRemovingDFS(type, x, y, z, newValue, oldValue, positionsToSpread);

  for (const auto& [position, value] : positionsToSpread) {
    LightSpreadingDFS(type, XYZ(position), value);
  }
}

void Chunk::MarkPositionDirty(glm::ivec3 localPosition) {
  if (IsInOtherChunk(localPosition.x, localPosition.y, localPosition.z)) {
    m_world.MarkPositionDirty(ToGlobalCoords(localPosition));
  } else if (localPosition.y >= 0 && localPosition.y < CHUNK_HEIGHT) {
    int index = GetSubchunkIndex(localPosition.y);
    m_dirtySubchunks.insert(index);
    m_world.MarkChunkDirty(this);
  }
}

void Chunk::MarkPositionAndNeighborsDirty(glm::ivec3 localPosition) {
  for (const glm::ivec3& offset : VoxelData::GetNeighborOffsetsAndOrigin()) {
    MarkPositionDirty(localPosition + offset);
  }
}

void Chunk::MarkPositionAndAllNeighborsDirty(glm::ivec3 localPosition) {
  for (int x = -1; x <= 1; x++) {
    for (int y = -1; y <= 1; y++) {
      for (int z = -1; z <= 1; z++) {
        MarkPositionDirty(localPosition + glm::ivec3 { x, y, z });
      }
    }
  }
}

void Chunk::CleanDirty() {
  for (int i : m_dirtySubchunks) {
    GenerateMeshForSubchunk(i);

    std::vector<float>& vertices = m_subchunkMeshesData[i].vertices;
    std::vector<unsigned int>& indices = m_subchunkMeshesData[i].indices;

    m_subchunkMeshes[i].SetData(vertices.data(), vertices.size(), indices.data(), indices.size());
  }
  m_dirtySubchunks.clear();
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

Blockstate Chunk::GetBlockstateAt(int localX, int localY, int localZ) {
  if (IsInsideChunk(localX, localY, localZ)) {
    return m_blockstates[PosToIndex(localX, localY, localZ)];
  } else if (localY >= 0 && localY < CHUNK_HEIGHT) {
    std::shared_ptr<Chunk> neighbor = GetNeighbor(localX, localZ);
    if (neighbor == nullptr) return Blocks::VOID_AIR;

    glm::ivec3 neighborCoords = ToNeighborCoords(localX, localY, localZ);
    return neighbor->GetBlockstateAt(neighborCoords.x, neighborCoords.y, neighborCoords.z);
  }
  return Blocks::VOID_AIR;
}

const Block& Chunk::GetBlockAt(int localX, int localY, int localZ) {
  return Block::FromBlockstate(GetBlockstateAt(localX, localY, localZ));
}

char Chunk::GetLightAt(LightType type, int localX, int localY, int localZ) {
  if (IsInsideChunk(localX, localY, localZ)) {
    return m_lights[PosToIndex(localX, localY, localZ)].GetLight(type);
  } else if (localY >= 0 && localY < CHUNK_HEIGHT) {
    std::shared_ptr<Chunk> neighbor = GetNeighbor(localX, localZ);
    if (neighbor == nullptr) return 0;

    glm::ivec3 neighborCoords = ToNeighborCoords(localX, localY, localZ);
    return neighbor->GetLightAt(type, neighborCoords.x, neighborCoords.y, neighborCoords.z);
  }
  return 0;
}

float Chunk::GetFixedLightAt(LightType type, int localX, int localY, int localZ) {
  if (localY < 0 || localY >= CHUNK_HEIGHT) return 0;
  const Block& block = Block::FromBlockstate(GetBlockstateAt(localX, localY, localZ));
  char lightLevel;
  if (block.IsSolid()) {
    lightLevel = -1;
  } else {
    lightLevel = GetLightAt(type, localX, localY, localZ);
  }
  float light = lightLevel / 15.0f;
  return light;
}

void Chunk::SetLightAt(LightType type, int localX, int localY, int localZ, char value) {
  if (IsInsideChunk(localX, localY, localZ)) {
    m_lights[PosToIndex(localX, localY, localZ)].SetLight(type, value);
  } else if (localY >= 0 && localY < CHUNK_HEIGHT) {
    std::shared_ptr<Chunk> neighbor = GetNeighbor(localX, localZ);
    if (neighbor == nullptr) return;

    glm::ivec3 neighborCoords = ToNeighborCoords(localX, localY, localZ);
    neighbor->SetLightAt(type, neighborCoords.x, neighborCoords.y, neighborCoords.z, value);
  }
}

void Chunk::SetBlockstateAt(int localX, int localY, int localZ, Blockstate value) {
  if (IsInsideChunk(localX, localY, localZ)) {
    m_blockstates[PosToIndex(localX, localY, localZ)] = value;
  } else if (localY >= 0 && localY < CHUNK_HEIGHT) {
    std::shared_ptr<Chunk> neighbor = GetNeighbor(localX, localZ);
    if (neighbor == nullptr) return;

    glm::ivec3 neighborCoords = ToNeighborCoords(localX, localY, localZ);
    neighbor->SetBlockstateAt(neighborCoords.x, neighborCoords.y, neighborCoords.z, value);
  }
}

void Chunk::SetActive(bool value) {
  m_active = value;
}

void Chunk::InvalidateMesh() {
  std::lock_guard<std::mutex> lock(m_stateMutex);
  m_generatedMesh = false;
  m_appliedMesh = false;
}

bool Chunk::HasAppliedMesh() const {
  std::lock_guard<std::mutex> lock(m_stateMutex);
  return m_appliedMesh;
}

bool Chunk::HasGeneratedTerrain() const {
  std::lock_guard<std::mutex> lock(m_stateMutex);
  return m_generatedTerrain;
}

bool Chunk::HasGeneratedMesh() const {
  std::lock_guard<std::mutex> lock(m_stateMutex);
  return m_generatedMesh;
}

bool Chunk::HasPropagatedLighting() const {
  std::lock_guard<std::mutex> lock(m_stateMutex);
  return m_propagatedLighting;
}

glm::ivec2 Chunk::GetChunkCoord() const {
  return m_chunkCoord;
}

inline int Chunk::PosToIndex(int localX, int localY, int localZ) const {
  // x  y  z
  return localZ + localY * CHUNK_WIDTH + localX * CHUNK_HEIGHT * CHUNK_WIDTH;
}

inline int Chunk::PosToIndex(const glm::ivec3& local) const {
  return local.z + local.y * CHUNK_WIDTH + local.x * CHUNK_HEIGHT * CHUNK_WIDTH;
}

bool Chunk::IsInsideChunk(int localX, int localY, int localZ) const {
  return localX >= 0 && localY >= 0 && localZ >= 0 && localX < CHUNK_WIDTH && localY < CHUNK_HEIGHT && localZ < CHUNK_WIDTH;
}

// Opposite of IsInsideChunk but doesn't check the Y coord
bool Chunk::IsInOtherChunk(int localX, int localY, int localZ) const {
  return localX < 0 || localZ < 0 || localX >= CHUNK_WIDTH || localZ >= CHUNK_WIDTH;
}

glm::ivec3 Chunk::ToGlobalCoords(int localX, int localY, int localZ) const {
  return {
    localX + CHUNK_WIDTH * m_chunkCoord.x,
    localY,
    localZ + CHUNK_WIDTH * m_chunkCoord.y
  };
}

glm::ivec3 Chunk::ToGlobalCoords(const glm::ivec3& local) const {
  return {
    local.x + CHUNK_WIDTH * m_chunkCoord.x,
    local.y,
    local.z + CHUNK_WIDTH * m_chunkCoord.y
  };
}

int Chunk::GetSubchunkIndex(int localY) const {
  return localY / Chunk::SUBCHUNK_HEIGHT;
}

glm::ivec3 Chunk::ToNeighborCoords(int localX, int localY, int localZ) const {
  return {
    MathUtil::Mod(localX, Chunk::CHUNK_WIDTH),
    localY,
    MathUtil::Mod(localZ, Chunk::CHUNK_WIDTH)
  };
}

char SkyBlockLight::GetLight(LightType type) {
  if (type == LightType::SKY) {
    return m_value1;
  } else {
    return m_value2;
  }
}

void SkyBlockLight::SetLight(LightType type, char value) {
  if (type == LightType::SKY) {
    m_value1 = value;
  } else {
    m_value2 = value;
  }
}