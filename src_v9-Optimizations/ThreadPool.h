#pragma once
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

/// <summary>
/// Thread pool that manages a fixed number of worker threads
/// Workers wait for tasks and execute them asynchronously
/// </summary>
class ThreadPool {
public:
    ThreadPool(size_t numThreads);
    ~ThreadPool();

    void enqueueTask(std::function<void()> task);
    bool isIdle() const;
    int getQueueSize() const;

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    std::mutex queueMutex;
    std::condition_variable condition;
    bool stop;
    std::atomic<int> activeTasks;

    void workerThread();
};
