#pragma once

#include <string>

#include "util/ClassMacros.h"

class Texture {

public:
  Texture();
  explicit Texture(const std::string& fileName, const std::string& extension = ".png");
  Texture(int width, int height, unsigned char* imageData, int mipmapLevels = 0);
  ~Texture();

  DELETE_COPY(Texture);

  // Move constructor
  Texture(Texture&& other);
  // Move assignment operator
  Texture& operator=(Texture&& other);

  void Use(unsigned int slot = 0) const;
  unsigned int GetID() const;

private:
  unsigned int m_textureID;
};