#pragma once

#include "../init/registry/RegistryItem.h"
#include <unordered_map>
#include "BlockTextures.h"

using Blockstate = unsigned char;

class Block : public RegistryItem {
public:
  Block() = default;
  explicit Block(const std::string& registryName);
  Block(const std::string& registryName, BlockTextures textures);

  const BlockTextures GetTextures() const;
  Blockstate GetBlockstate() const;
  operator Blockstate() const;

  void Initialize();

  static const Block& FromBlockstate(char blockstate);

  Block& NotSolid();
  Block& Air();
  Block& LightLevel(char lightLevel);

  bool IsSolid() const;
  bool IsAir() const;
  char GetLightLevel() const;

private:
  BlockTextures m_textures;
  Blockstate m_blockstate;

  // Properties
  bool m_isSolid = true;
  bool m_isAir = false;
  char m_lightLevel = 0;

  static char s_blockstateIndex;
  static std::unordered_map<char, Block*> s_blockstateMap;
};