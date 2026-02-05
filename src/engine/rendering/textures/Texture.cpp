#include "Texture.h"

#include <glad/glad.h>
#include <stb/stb_image.h>
#include <string>
#include "util/Logging.h"

Texture::Texture() {
  glGenTextures(1, &m_textureID);
  glBindTexture(GL_TEXTURE_2D, m_textureID);
}

Texture::Texture(const std::string& fileName, const std::string& extension) {
  glGenTextures(1, &m_textureID);
  glBindTexture(GL_TEXTURE_2D, m_textureID);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 3); // Max level before a 16x16 texture is smaller than 1px

  int width, height, channels;
  stbi_set_flip_vertically_on_load(true);
  std::string filePath = "res/textures/" + fileName + extension;

  unsigned char* image_data = stbi_load(filePath.c_str(), &width, &height, &channels, 4);

  if (image_data != nullptr) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    LOG(ERROR) << "Failed to load texture: " << fileName + extension;
  }

  stbi_image_free(image_data);
}

Texture::Texture(int width, int height, unsigned char* imageData, int mipmapLevels) {
  glGenTextures(1, &m_textureID);
  glBindTexture(GL_TEXTURE_2D, m_textureID);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipmapLevels);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
  glGenerateMipmap(GL_TEXTURE_2D);
}

Texture::~Texture() {
  if (m_textureID != 0) {
    glDeleteTextures(1, &m_textureID);
  }
}

Texture::Texture(Texture&& other) {
  m_textureID = other.m_textureID;
  other.m_textureID = 0;
}

Texture& Texture::operator=(Texture&& other) {
  glDeleteTextures(1, &m_textureID);
  m_textureID = other.m_textureID;
  other.m_textureID = 0;
  return *this;
}

void Texture::Use(unsigned int slot) const {
  glActiveTexture(GL_TEXTURE0 + slot);
  glBindTexture(GL_TEXTURE_2D, m_textureID);
}

unsigned int Texture::GetID() const {
  return m_textureID;
}
