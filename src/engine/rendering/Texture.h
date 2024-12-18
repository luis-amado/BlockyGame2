#pragma once

#include <string>

#include "util/ClassMacros.h"

class Texture {

public:
  explicit Texture(const std::string& fileName);
  ~Texture();

  ONLY_MOVE_ID(Texture, m_textureID);

  void Use(unsigned int slot = 0) const;

private:
  unsigned int m_textureID;
};