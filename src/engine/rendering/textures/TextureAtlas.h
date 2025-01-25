#pragma once

#include <glad/glad.h>

#include <unordered_map>
#include <string>
#include <glm/vec2.hpp>
#include "Texture.h"

#include "util/ClassMacros.h"

class TextureAtlas {
public:

  struct TextureLocation {
    glm::vec2 pos;
    float size;
  };

  struct TextureCoords {
    float x0, y0, x1, y1;
  };

  TextureAtlas() = default;
  TextureAtlas(Texture& texture, const std::unordered_map<std::string, TextureLocation>& textureMap);

  ALLOW_MOVE(TextureAtlas) {
    m_texture = std::move(other.m_texture);
    m_textureMap = std::move(other.m_textureMap);
    return *this;
  };

  void Use(unsigned int slot = GL_TEXTURE0) const;
  TextureCoords GetTextureCoords(const std::string& textureName) const;

private:
  std::optional<Texture> m_texture;
  std::unordered_map<std::string, TextureLocation> m_textureMap;

};