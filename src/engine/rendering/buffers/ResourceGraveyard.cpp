#include <glad/glad.h>
#include "ResourceGraveyard.h"

ResourceGraveyard& ResourceGraveyard::GetInstance() {
  static ResourceGraveyard instance;
  return instance;
}

void ResourceGraveyard::QueueVAOForDeletion(unsigned int vao) {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_vaosToDelete.push_back(vao);
}

void ResourceGraveyard::QueueVBOForDeletion(unsigned int vbo) {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_vbosToDelete.push_back(vbo);
}

void ResourceGraveyard::Flush() {
  std::lock_guard<std::mutex> lock(m_mutex);

  if (!m_vbosToDelete.empty()) {
    glDeleteBuffers(m_vbosToDelete.size(), m_vbosToDelete.data());
  }
  if (!m_vaosToDelete.empty()) {
    glDeleteVertexArrays(m_vaosToDelete.size(), m_vaosToDelete.data());
  }

  m_vbosToDelete.clear();
  m_vaosToDelete.clear();

}
