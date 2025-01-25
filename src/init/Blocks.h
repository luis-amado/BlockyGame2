#pragma once

#include "registry/Registry.h"
#include "../block/Block.h"

#include "rendering/textures/TextureAtlas.h"
#include <optional>

class Blocks {
public:
  static const Block& AIR;
  static const Block& VOID_AIR;
  static const Block& CAVE_AIR;
  static const Block& DIRT;
  static const Block& GRASS;
  static const Block& STONE;
  static const Block& BEDROCK;

  static void InitializeBlocks();
  static void GenerateBlockAtlas();

  static const TextureAtlas& GetAtlas();

private:
  static Registry<Block> s_registry;
  static std::optional<TextureAtlas> s_atlas;
};