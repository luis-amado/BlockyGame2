#pragma once

#include <string>

#ifndef RESOURCES_PATH
#define RESOURCES_PATH "./res/" 
#endif

namespace FileUtil {

std::string ReadSource(const std::string& fileLocation);

} // namespace FileUtil