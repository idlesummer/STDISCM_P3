#pragma once
#include "TaskQueue.hpp"
#include <vector>
#include <thread>
#include <functional>
#include <atomic>
#include <iostream>

using namespace std;


class ThreadPool {
private:
    size_t nthreads;
    vector<thread> workers;
    TaskQueue<function<void()>> taskQueue;  // Clean abstraction!
    atomic<int> activeTasks;

public:
    ThreadPool(size_t nthreads)
        : nthreads(nthreads),
            workers(),
            taskQueue(),
            activeTasks(0) {

        for (auto i = size_t(0); i < nthreads; i++)
            this->workers.emplace_back([this] { this->workerLoop(); });
    }

    ~ThreadPool() {
        // Signal shutdown
        this->taskQueue.shutdown();

        for (auto& worker : this->workers) {
            if (worker.joinable())
                worker.join();
        }
    }

    void enqueue(function<void()> task) {
        this->taskQueue.push(task);
    }

    auto isIdle() const { return this->taskQueue.empty() && this->activeTasks == 0; }
    auto getQueueSize() const { return this->taskQueue.size(); }
    auto getThreadCount() const { return this->nthreads; }

private:
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
