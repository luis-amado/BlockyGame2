#include "Block.h"

#include "util/Logging.h"

char Block::s_blockstateIndex = 0;
std::unordered_map<char, Block*> Block::s_blockstateMap;

Block::Block(const std::string& registryName) : RegistryItem(registryName) {
  m_blockstate = s_blockstateIndex++;
}

Block::Block(const std::string& registryName, BlockTextures textures)
  : RegistryItem(registryName), m_textures(textures) {

  m_blockstate = s_blockstateIndex++;
}

const BlockTextures Block::GetTextures() const {
  return m_textures;
}

char Block::GetBlockstate() const {
  return m_blockstate;
}

Block::operator char() const {
  return m_blockstate;
}

void Block::Initialize() {
  s_blockstateMap[m_blockstate] = this;
}

const Block& Block::FromBlockstate(char blockstate) {
  return *s_blockstateMap.at(blockstate);
}

Block& Block::NotSolid() {
  m_isSolid = false;
  return *this;
}

Block& Block::Air() {
  m_isAir = true;
  return *this;
}

bool Block::IsSolid() const {
  return m_isSolid;
}

bool Block::IsAir() const {
  return m_isAir;
}
