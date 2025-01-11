#pragma once

#include <shared_mutex>

template <typename T>
class ThreadSafeWrapper {
public:
  ThreadSafeWrapper() = default;
  ThreadSafeWrapper(T value) : m_value(value) {}

  T Get() const {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    return m_value;
  }

  void Set(T value) {
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_value = value;
  }
private:
  mutable std::shared_mutex m_mutex;
  T m_value;
};