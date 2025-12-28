#pragma once

#include <queue>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <functional>

template <typename T, typename Comparator = std::greater<T>>
class ThreadSafePriorityQueue {
public:

  ThreadSafePriorityQueue() = default;

  void push(const T& item) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_queue.push(item);
    m_condition.notify_one();
  }

  void push(T&& item) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_queue.push(std::move(item));
    m_condition.notify_one();
  }

  bool pop(T& item) {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_condition.wait(lock, [this]() { return !m_queue.empty() || m_stop; });

    if (m_queue.empty() || m_stop) {
      return false;
    }

    item = m_queue.top();
    m_queue.pop();
    return true;
  }

  bool empty() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_queue.empty();
  }

  void stop() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_stop = true;
    m_condition.notify_all();
  }

  void start() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_stop = false;
  }

  int size() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_queue.size();
  }

  void clear() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_queue = {};
  }

private:
  mutable std::mutex m_mutex;
  std::condition_variable m_condition;
  bool m_stop = false;

  std::priority_queue<T, std::vector<T>, Comparator> m_queue;
};
