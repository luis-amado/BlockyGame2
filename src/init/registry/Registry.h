#pragma once

#include <unordered_map>
#include <vector>
#include <string>

// A registry is just a container that can query items which have a registryName
template <class T>
class Registry {
public:
  Registry() = default;

  const T& Register(T item) {
    std::string name = item.GetRegistryName();
    m_map[name] = item;
    return m_map.at(name);
  }

  const T& Get(const std::string& registryName) const {
    return m_map.at(registryName);
  }

  std::vector<T*> GetAll() {
    std::vector<T*> items;
    for (auto& pair : m_map) {
      items.push_back(&pair.second);
    }
    return items;
  }

private:
  std::unordered_map<std::string, T> m_map;

};