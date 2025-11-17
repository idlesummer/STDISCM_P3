#pragma once
#include "TaskQueue.hpp"
#include <vector>
#include <thread>
#include <functional>
#include <atomic>
#include <iostream>

class ThreadPool {
public:
    ThreadPool(size_t numThreads)
        : workers(),
          taskQueue(),
          activeTasks(0) {

        std::cout << "[ThreadPool] Creating pool with " << numThreads << " workers" << std::endl;
        for (size_t i = 0; i < numThreads; i++)
            this->workers.emplace_back([this] { this->workerLoop(); });
    }

    ~ThreadPool() {
        this->taskQueue.shutdown();  // Signal shutdown

        for (auto& worker : this->workers)
            if (worker.joinable())
                worker.join();

        std::cout << "[ThreadPool] All workers stopped" << std::endl;
    }

    void enqueue(std::function<void()> task) {
        this->taskQueue.push(task);  // One line - beautiful!
    }

    bool isIdle() const {
        return this->taskQueue.empty() && this->activeTasks == 0;
    }

    int getQueueSize() const {
        return this->taskQueue.size();
    }

private:
    std::vector<std::thread> workers;
    TaskQueue<std::function<void()>> taskQueue;  // Clean abstraction!
    std::atomic<int> activeTasks;

    void workerLoop() {
        // Loop while tasks are available; pop() returns nullopt on shutdown
        while (auto taskOpt = this->taskQueue.pop()) {
            auto task = taskOpt.value();
            this->activeTasks++;
            task();
            this->activeTasks--;
        }
    }
};
