#include "TextureAtlas.h"

TextureAtlas::TextureAtlas(Texture& texture, const std::unordered_map<std::string, TextureLocation>& textureMap)
  : m_texture(std::move(texture)), m_textureMap(std::move(textureMap)) {}

void TextureAtlas::Use(unsigned int slot) const {
  m_texture->Use(slot);
}

TextureAtlas::TextureCoords TextureAtlas::GetTextureCoords(const std::string& textureName) const {

  const TextureLocation* location;

  if (m_textureMap.count(textureName)) {
    location = &m_textureMap.at(textureName);
  } else {
    location = &m_textureMap.at("error");
  }

  TextureCoords coords;
  coords.x0 = location->pos.x;
  coords.y0 = location->pos.y;
  coords.x1 = location->pos.x + location->size;
  coords.y1 = location->pos.y + location->size;

  return coords;
}
