#pragma once

#include <unordered_map>
#include <string>

// A registry is just a container that can query items which have a registryName
template <class T>
class Registry {
public:
  Registry() = default;

  const T& Register(T item) {
    m_map[item.GetRegistryName()] = item;
    return m_map.at(item.GetRegistryName());
  }

  const T& Get(const std::string& registryName) const {
    return m_map.at(registryName);
  }

  std::unordered_map<std::string, T>& GetAll() {
    return m_map;
  }

private:
  std::unordered_map<std::string, T> m_map;

};