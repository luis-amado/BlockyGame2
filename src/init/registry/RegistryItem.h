#pragma once

#include <string>

class RegistryItem {
public:

  RegistryItem() = default;
  RegistryItem(const std::string& registryName);
  std::string GetRegistryName() const;

protected:
  std::string m_registryName;
};