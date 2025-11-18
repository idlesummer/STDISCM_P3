#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>

using namespace std;


template<typename T>
class TaskQueue {
private:
  queue<T> queue_;
  mutable mutex mutex_;  // mutable for const methods
  condition_variable cv_;
  bool shutdown_;

public:
  TaskQueue()
    : queue_(),
      mutex_(),
      cv_(),
      shutdown_(false) {}

  ~TaskQueue() = default;
  TaskQueue(const TaskQueue&) = delete;
  TaskQueue& operator=(const TaskQueue&) = delete;

  void push(T item) {
    {
      auto lock = lock_guard<mutex>(this->mutex_);
      this->queue_.push(move(item));
    }
    this->cv_.notify_one();
  }

  auto pop() -> optional<T> {
    auto lock = unique_lock<mutex>(this->mutex_);

    // Wait until queue has items or shutdown is signaled
    this->cv_.wait(lock, [this] { return !this->queue_.empty() || this->shutdown_; });

    // If shutdown and no items, return nullopt
    if (this->shutdown_ && this->queue_.empty())
      return nullopt;

    auto item = move(this->queue_.front());
    this->queue_.pop();
    return item;
  }

  /** Returns nullopt if queue is empty. */
  auto tryPop() -> optional<T> {
    auto lock = lock_guard<mutex>(this->mutex_);
    if (this->queue_.empty())
      return nullopt;

    auto item = move(this->queue_.front());
    this->queue_.pop();
    return item;
  }

  void shutdown() {
    {
      auto lock = lock_guard<mutex>(this->mutex_);
      this->shutdown_ = true;
    }
    this->cv_.notify_all();
  }

  bool empty() const {
    auto lock = lock_guard<mutex>(this->mutex_);
    return this->queue_.empty();
  }

  auto size() const {
    auto lock = lock_guard<mutex>(this->mutex_);
    return static_cast<int>(this->queue_.size());
  }

  auto isShutdown() const {
    auto lock = lock_guard<mutex>(this->mutex_);
    return this->shutdown_;
  }
};
