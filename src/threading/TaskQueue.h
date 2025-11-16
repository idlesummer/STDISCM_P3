#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>

/**
 * Thread-safe blocking queue for producer-consumer patterns.
 * Abstracts away mutex and condition variable operations.
 *
 * Template parameter T: Type of items in the queue
 */
template<typename T>
class TaskQueue {
public:
    TaskQueue()
        : queue_(),
          mutex_(),
          cv_(),
          shutdown_(false) {}

    ~TaskQueue() = default;

    // Disable copy (mutex is not copyable)
    TaskQueue(const TaskQueue&) = delete;
    TaskQueue& operator=(const TaskQueue&) = delete;

    /**
     * Push an item to the queue and notify one waiting thread
     */
    void push(T item) {
        {
            auto lock = std::lock_guard<std::mutex>(mutex_);
            queue_.push(std::move(item));
        }
        cv_.notify_one();
    }

    /**
     * Blocking pop - waits until an item is available or queue is shutdown
     * Returns std::nullopt if queue is shutdown and empty
     */
    std::optional<T> pop() {
        auto lock = std::unique_lock<std::mutex>(mutex_);

        // Wait until queue has items or shutdown is signaled
        cv_.wait(lock, [this] {
            return !queue_.empty() || shutdown_;
        });

        // If shutdown and no items, return nullopt
        if (shutdown_ && queue_.empty()) {
            return std::nullopt;
        }

        auto item = std::move(queue_.front());
        queue_.pop();
        return item;
    }

    /**
     * Non-blocking pop - returns immediately
     * Returns std::nullopt if queue is empty
     */
    std::optional<T> tryPop() {
        auto lock = std::lock_guard<std::mutex>(mutex_);

        if (queue_.empty()) {
            return std::nullopt;
        }

        auto item = std::move(queue_.front());
        queue_.pop();
        return item;
    }

    /**
     * Check if queue is empty (thread-safe)
     */
    bool empty() const {
        auto lock = std::lock_guard<std::mutex>(mutex_);
        return queue_.empty();
    }

    /**
     * Get queue size (thread-safe)
     */
    int size() const {
        auto lock = std::lock_guard<std::mutex>(mutex_);
        return static_cast<int>(queue_.size());
    }

    /**
     * Signal shutdown - wakes all waiting threads
     * After shutdown, pop() will return nullopt when queue is empty
     */
    void shutdown() {
        {
            auto lock = std::lock_guard<std::mutex>(mutex_);
            shutdown_ = true;
        }
        cv_.notify_all();
    }

    /**
     * Check if shutdown has been signaled
     */
    bool isShutdown() const {
        auto lock = std::lock_guard<std::mutex>(mutex_);
        return shutdown_;
    }

private:
    std::queue<T> queue_;
    mutable std::mutex mutex_;  // mutable for const methods
    std::condition_variable cv_;
    bool shutdown_;
};
