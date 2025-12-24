#pragma once

#include <shared_mutex>

template <typename T>
class ThreadSafeReference {
public:
  ThreadSafeReference() = default;
  ThreadSafeReference(T* value) : m_value(value) {
    if (m_value == nullptr) return;
    m_value->a_references++;
  }

  ~ThreadSafeReference() {
    if (m_value == nullptr) return;

    m_value->a_references--;
    if (m_value->a_references == 0) {
      m_value->OnStopReference();
    }
  }

  T* operator->() {
    return m_value;
  }

  T* Get() const {
    return m_value;
  }
private:
  T* m_value;
};