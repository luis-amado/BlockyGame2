#include "Texture.h"

#include <glad/glad.h>
#include <stb/stb_image.h>
#include <string>
#include "util/Logging.h"

Texture::Texture(const std::string& fileName) {
  glGenTextures(1, &m_textureID);
  glBindTexture(GL_TEXTURE_2D, m_textureID);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4); // Max level before a 16x16 texture is smaller than 1px

  int width, height, channels;
  stbi_set_flip_vertically_on_load(true);
  std::string filePath = "res/textures/" + fileName;
  unsigned char* image_data = stbi_load(filePath.c_str(), &width, &height, &channels, 0);
  LOG(INFO) << "Channels: " << channels;

  if (image_data != nullptr) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
    glGenerateMipmap(GL_TEXTURE_2D);
    LOG(INFO) << "Loaded texture: " << fileName;
  } else {
    LOG(ERROR) << "Failed to load texture: " << fileName;
  }

  stbi_image_free(image_data);
}

Texture::~Texture() {
  if (m_textureID != 0) {
    glDeleteTextures(1, &m_textureID);
  }
}

void Texture::Use(unsigned int slot) const {
  glActiveTexture(GL_TEXTURE0 + slot);
  glBindTexture(GL_TEXTURE_2D, m_textureID);
}