#include "Blocks.h"
#include <glad/glad.h>
#include "rendering/textures/TextureAtlasBuilder.h"
#include <unordered_set>
#include "../block/Block.h"
#include "../block/BlockTextures.h"
#include "util/Logging.h"

Registry<Block> Blocks::s_registry;
std::unique_ptr<TextureAtlas> Blocks::s_atlas = nullptr;

const Block& Blocks::AIR = s_registry.Register(Block("air").Air().NotSolid());
const Block& Blocks::VOID_AIR = s_registry.Register(Block("void_air").Air().NotSolid());
const Block& Blocks::CAVE_AIR = s_registry.Register(Block("cave_air").Air().NotSolid());
const Block& Blocks::DIRT = s_registry.Register(Block("dirt", BlockTextures::All("block/dirt")));
const Block& Blocks::GRASS = s_registry.Register(Block("grass", BlockTextures::SideUpDown("block/grass_side", "block/grass_top", "block/dirt")));
const Block& Blocks::STONE = s_registry.Register(Block("stone", BlockTextures::All("block/stone")));
const Block& Blocks::COAL_ORE = s_registry.Register(Block("coal_ore", BlockTextures::All("block/coal_ore")));
const Block& Blocks::IRON_ORE = s_registry.Register(Block("iron_ore", BlockTextures::All("block/iron_ore")));
const Block& Blocks::BEDROCK = s_registry.Register(Block("bedrock", BlockTextures::All("block/bedrock")));
const Block& Blocks::OAK_LOG = s_registry.Register(Block("oak_log", BlockTextures::SideEnd("block/oak_log", "block/oak_log_top")));
const Block& Blocks::OAK_LEAVES = s_registry.Register(Block("oak_leaves", BlockTextures::All("block/oak_leaves")).NotSolid());

void Blocks::InitializeBlocks() {
  for (auto& [registryName, block] : s_registry.GetAll()) {
    block.Initialize();
  }
}

void Blocks::GenerateBlockAtlas() {
  TextureAtlasBuilder atlasBuilder;

  std::unordered_set<std::string> addedTextures { "" };

  for (const auto& [registryName, block] : s_registry.GetAll()) {
    for (const std::string& textureName : block.GetTextures().GetAll()) {
      if (!addedTextures.count(textureName)) {
        atlasBuilder.AddImageFile(textureName);
        addedTextures.insert(textureName);
      }
    }
  }

  s_atlas = atlasBuilder.Build();
}

const TextureAtlas& Blocks::GetAtlas() {
  if (s_atlas == nullptr) {
    LOG(FATAL) << "No block texture atlas exists";
  }
  return *s_atlas;
}
