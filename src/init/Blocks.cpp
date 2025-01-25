#include "Blocks.h"
#include <glad/glad.h>
#include "rendering/textures/TextureAtlasBuilder.h"
#include <unordered_set>
#include "../block/Block.h"
#include "../block/BlockTextures.h"

Registry<Block> Blocks::s_registry;
std::optional<TextureAtlas> Blocks::s_atlas;

const Block& Blocks::AIR = s_registry.Register(Block("air").NotSolid());
const Block& Blocks::VOID_AIR = s_registry.Register(Block("void_air").NotSolid());
const Block& Blocks::DIRT = s_registry.Register(Block("dirt", BlockTextures::All("dirt")));
const Block& Blocks::GRASS = s_registry.Register(Block("grass", BlockTextures::SideUpDown("grass_side", "grass_top", "dirt")));
const Block& Blocks::STONE = s_registry.Register(Block("stone", BlockTextures::All("stone")));
const Block& Blocks::BEDROCK = s_registry.Register(Block("bedrock", BlockTextures::All("bedrock")));

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
  return *s_atlas;
}
