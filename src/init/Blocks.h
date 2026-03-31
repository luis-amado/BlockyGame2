#pragma once

#include "registry/Registry.h"
#include "../block/Block.h"

#include "rendering/textures/TextureAtlas.h"
#include <optional>


#define BLOCK(identifier, ...) inline static const Block& identifier = s_registry.Register(__VA_ARGS__);
class Blocks {
private:
  inline static Registry<Block> s_registry;
  inline static std::unique_ptr<TextureAtlas> s_atlas = nullptr;

public:

  BLOCK(AIR, Block("air").Air().NotSolid());
  BLOCK(VOID_AIR, Block("void_air").Air().NotSolid());
  BLOCK(CAVE_AIR, Block("cave_air").Air().NotSolid());
  BLOCK(DIRT, Block("dirt", BlockTextures::All("block/dirt")));
  BLOCK(GRASS, Block("grass", BlockTextures::SideUpDown("block/grass_side", "block/grass_top", "block/dirt")));
  BLOCK(STONE, Block("stone", BlockTextures::All("block/stone")));
  BLOCK(COAL_ORE, Block("coal_ore", BlockTextures::All("block/coal_ore")));
  BLOCK(IRON_ORE, Block("iron_ore", BlockTextures::All("block/iron_ore")));
  BLOCK(BEDROCK, Block("bedrock", BlockTextures::All("block/bedrock")));
  BLOCK(OAK_LOG, Block("oak_log", BlockTextures::SideEnd("block/oak_log", "block/oak_log_top")));
  BLOCK(OAK_PLANKS, Block("oak_planks", BlockTextures::All("block/oak_planks")));
  BLOCK(OAK_LEAVES, Block("oak_leaves", BlockTextures::All("block/oak_leaves")).NotSolid());
  BLOCK(GLOWSTONE, Block("glowstone", BlockTextures::All("block/glowstone")).LightLevel(15));
  BLOCK(GLASS, Block("glass", BlockTextures::All("block/glass")).NotSolid().TransparentHidesNeighbors());

  static void InitializeBlocks();
  static void GenerateBlockAtlas();
  static const TextureAtlas& GetAtlas();
};

#undef BLOCK