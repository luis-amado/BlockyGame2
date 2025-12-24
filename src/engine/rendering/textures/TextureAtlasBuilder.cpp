#include "TextureAtlasBuilder.h"

#include <stb/stb_image.h>
#include "util/MathUtil.h"
#include "util/Logging.h"

TextureAtlasBuilder::TextureAtlasBuilder() {
  AddImageFile("error");
}

void TextureAtlasBuilder::AddImageFile(const std::string& fileName, const std::string& extension) {
  int width, height, channels;
  std::string filePath = "res/textures/" + fileName + extension;
  stbi_set_flip_vertically_on_load(true);
  unsigned char* bytes = stbi_load(filePath.c_str(), &width, &height, &channels, 4);

  if (bytes == nullptr) {
    LOG(ERROR) << "Failed to find image: " << fileName;
    return;
  }

  if (width != height || !MathUtil::IsPowerOf2(width)) {
    LOG(ERROR) << "Image dimensions for: " << fileName << " are not valid";
    return;
  }

  std::vector<unsigned char> bytes_vector(width * height * 4);
  memcpy(bytes_vector.data(), bytes, width * height * 4);

  m_images.push_back({ width, fileName, std::move(bytes_vector) });

  stbi_image_free(bytes);
}

void TextureAtlasBuilder::AddImageBytes(const std::string& imageName, std::vector<unsigned char> bytes, int size) {
  Image image = { size, imageName, bytes };
  m_images.push_back(image);
}

// TODO: Support different sized textures if ever needed
TextureAtlas TextureAtlasBuilder::Build() const {
  // Create the texture by combining the different images

  const int TEXTURE_SIZE = 16;

  int minRequiredSize = std::ceil(std::sqrt((float)m_images.size())) * TEXTURE_SIZE;
  int textureMapSize = MathUtil::NearestPowerOf2(minRequiredSize);
  int texturesPerRow = textureMapSize / TEXTURE_SIZE;

  std::vector<unsigned char> textureAtlasBytes(textureMapSize * textureMapSize * 4, 0);

  for (int y = 0; y < textureMapSize; y++) {
    for (int x = 0; x < textureMapSize; x++) {
      int textureIndex = x / TEXTURE_SIZE + ((textureMapSize - 1 - y) / TEXTURE_SIZE) * texturesPerRow;
      if (textureIndex >= m_images.size()) continue;

      const std::vector<unsigned char>& bytes = m_images.at(textureIndex).bytes;
      int starterIndex = (x + y * textureMapSize) * 4;

      int localX = x % TEXTURE_SIZE;
      int localY = y % TEXTURE_SIZE;
      int localStarterIndex = (localX + localY * TEXTURE_SIZE) * 4;

      textureAtlasBytes[starterIndex + 0] = bytes[localStarterIndex + 0];
      textureAtlasBytes[starterIndex + 1] = bytes[localStarterIndex + 1];
      textureAtlasBytes[starterIndex + 2] = bytes[localStarterIndex + 2];
      textureAtlasBytes[starterIndex + 3] = bytes[localStarterIndex + 3];
    }
  }

  Texture texture(textureMapSize, textureMapSize, textureAtlasBytes.data(), /*mipmapLevels=*/3);

  // Create the coordinates mapping
  float size = 1.0f / texturesPerRow;
  std::unordered_map<std::string, TextureAtlas::TextureLocation> textureMapping;

  for (int i = 0; i < m_images.size(); i++) {
    int row = i / texturesPerRow;
    int col = i % texturesPerRow;

    float x0 = col * size;
    float y0 = 1.0f - ((row + 1) * size);

    textureMapping[m_images[i].name] = { {x0,y0}, size };
  }

  return TextureAtlas(texture, textureMapping);
}
