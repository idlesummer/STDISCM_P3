#pragma once
#include "TaskQueue.h"
#include <vector>
#include <thread>
#include <functional>
#include <atomic>

class ThreadPool {
public:
    ThreadPool(size_t numThreads);
    ~ThreadPool();

    void enqueue(std::function<void()> task);
    bool isIdle() const;
    int getQueueSize() const;

private:
    std::vector<std::thread> workers;
    TaskQueue<std::function<void()>> taskQueue;  // Clean abstraction!
    std::atomic<int> activeTasks;

    void workerLoop();
};
