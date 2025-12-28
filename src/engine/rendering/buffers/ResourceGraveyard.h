#include <vector>
#include <mutex>

class ResourceGraveyard {
public:
  static ResourceGraveyard& GetInstance();

  void QueueVAOForDeletion(unsigned int vao);
  void QueueVBOForDeletion(unsigned int vbo);

  void Flush();

private:
  std::mutex m_mutex;
  std::vector<unsigned int> m_vaosToDelete;
  std::vector<unsigned int> m_vbosToDelete;
};