#pragma once

#include "../init/registry/RegistryItem.h"
#include <unordered_map>
#include "BlockTextures.h"

class Block : public RegistryItem {
public:
  Block() = default;
  explicit Block(const std::string& registryName);
  Block(const std::string& registryName, BlockTextures textures);

  const BlockTextures GetTextures() const;
  char GetBlockstate() const;
  operator char() const;

  void Initialize();

  static const Block& FromBlockstate(char blockstate);

  Block& NotSolid();

  bool IsSolid() const;

private:
  BlockTextures m_textures;
  char m_blockstate;
  bool m_isSolid = true;

  static char s_blockstateIndex;
  static std::unordered_map<char, Block*> s_blockstateMap;
};