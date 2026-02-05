#include "FileUtil.h"

#include <fstream>
#include <sstream>
#include "Logging.h"

namespace FileUtil {

std::string ReadSource(const std::string& fileLocation) {
  std::ifstream file(RESOURCES_PATH + fileLocation);
  if (!file.is_open()) {
    LOG(ERROR) << "Failed to find file: " << fileLocation;
  }
  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

} // namespace FileUtil