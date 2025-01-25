#pragma once

#include <vector>
#include <string>
#include "TextureAtlas.h"

class TextureAtlasBuilder {
public:
  TextureAtlasBuilder();

  void AddImageFile(const std::string& fileName, const std::string& extension = ".png");
  void AddImageBytes(const std::string& imageName, std::vector<unsigned char> bytes, int size);

  TextureAtlas Build() const;

private:

  struct Image {
    int size;
    std::string name;
    std::vector<unsigned char> bytes;
  };

  std::vector<Image> m_images;
};