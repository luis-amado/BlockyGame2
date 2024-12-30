#pragma once

#include <unordered_map>
#include <glm/vec2.hpp>
#include "util/GlmExtensions.h"
#include "util/ClassMacros.h"
#include "rendering/Shader.h"
#include "Chunk.h"

class World {
public:
  DELETE_COPY(World);

  World();

  bool GetBlockstateAt(int globalX, int globalY, int globalZ) const;
  glm::ivec2 GetChunkCoord(int globalX, int globalZ) const;
  glm::ivec3 ToLocalCoords(int globalX, int globalY, int globalZ) const;

  void Draw(Shader& shader) const;
private:
  std::unordered_map<glm::ivec2, std::unique_ptr<Chunk>, IVec2Hash, IVec2Equal> m_chunks;

  bool IsInsideWorld(int globalX, int globalY, int globalZ) const;
};