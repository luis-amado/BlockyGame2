#pragma once

#include <string>

#include "util/ClassMacros.h"

class Texture {

public:
  explicit Texture(const std::string& fileName);
  Texture(int width, int height, unsigned char* image_data);
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