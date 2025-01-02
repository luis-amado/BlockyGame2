#pragma once

#include <unordered_map>
#include <shared_mutex>
#include <optional>
#include <functional>

template <typename Key, typename Value, typename Hasher = std::hash<Key>, typename Comparator = std::equal_to<Key>>
class ThreadSafeUnorderedMap {
public:
  void insert(const Key& key, const Value& value) {
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_map[key] = value;
  }

  std::optional<Value> get(const Key& key) const {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    auto it = m_map.find(key);
    if (it != m_map.end()) {
      return it->second;
    }
    return std::nullopt;
  }

  bool contains(const Key& key) const {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    return m_map.find(key) != m_map.end();
  }

  void erase(const Key& key) {
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_map.erase(key);
  }

  void clear() {
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_map.clear();
  }

  size_t size() const {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    return m_map.size();
  }

  void forEach(const std::function<void(const Key&, const Value&)>& func) const {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    for (const auto& [key, value] : m_map) {
      func(key, value);
    }
  }

private:
  mutable std::shared_mutex m_mutex;
  std::unordered_map<Key, Value, Hasher, Comparator> m_map;
};