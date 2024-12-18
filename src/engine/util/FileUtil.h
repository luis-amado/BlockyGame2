#pragma once

#include <fstream>
#include <sstream>
#include <string>

namespace FileUtil {

std::string ReadSource(const std::string& fileLocation) {
  std::ifstream file(fileLocation);
  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

} // namespace FileUtil