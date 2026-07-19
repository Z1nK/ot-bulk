#pragma once

#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>
#include <utility>

// Thread-safe queue used to hand lines off from the producer (stdin reader)
// to the consumer (parser/executor) thread.
template <typename T>
class BlockingQueue {
public:
  void push(T value) {
    {
      std::lock_guard<std::mutex> lock(mutex_);
      queue_.push(std::move(value));
    }
    cv_.notify_one();
  }

  // Signals that no more items will be pushed. Wakes up any pending pop().
  void close() {
    {
      std::lock_guard<std::mutex> lock(mutex_);
      closed_ = true;
    }
    cv_.notify_all();
  }

  // Blocks until an item is available or the queue is closed and drained,
  // in which case it returns std::nullopt.
  std::optional<T> pop() {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [this] { return !queue_.empty() || closed_; });
    if (queue_.empty()) {
      return std::nullopt;
    }
    T value = std::move(queue_.front());
    queue_.pop();
    return value;
  }

private:
  std::mutex mutex_;
  std::condition_variable cv_;
  std::queue<T> queue_;
  bool closed_ = false;
};
