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

Blockstate Block::GetBlockstate() const {
  return m_blockstate;
}

Block::operator Blockstate() const {
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

Block& Block::LightLevel(char lightLevel) {
  m_lightLevel = lightLevel;
  return *this;
}

Block& Block::TransparentHidesNeighbors() {
  m_transparentHideNeighbors = true;
  return *this;
}

bool Block::IsSolid() const {
  return m_isSolid;
}

bool Block::ShouldHideNeighbors() const {
  return m_transparentHideNeighbors;
}

bool Block::IsAir() const {
  return m_isAir;
}

char Block::GetLightLevel() const {
  return m_lightLevel;
}
