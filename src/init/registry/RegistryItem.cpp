#include "RegistryItem.h"

RegistryItem::RegistryItem(const std::string& registryName)
  : m_registryName(registryName) {}

std::string RegistryItem::GetRegistryName() const {
  return m_registryName;
}
