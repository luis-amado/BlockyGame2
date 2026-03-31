#include "Blocks.h"
#include <glad/glad.h>
#include "rendering/textures/TextureAtlasBuilder.h"
#include <unordered_set>
#include "../block/Block.h"
#include "../block/BlockTextures.h"
#include "util/Logging.h"

void Blocks::InitializeBlocks() {
  for (auto& block : s_registry.GetAll()) {
    block->Initialize();
  }
}

void Blocks::GenerateBlockAtlas() {
  TextureAtlasBuilder atlasBuilder;

  std::unordered_set<std::string> addedTextures { "" };

  for (const auto& block : s_registry.GetAll()) {
    for (const std::string& textureName : block->GetTextures().GetAll()) {
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
