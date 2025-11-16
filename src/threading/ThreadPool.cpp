#include "ThreadPool.h"
#include <iostream>

using namespace std;


ThreadPool::ThreadPool(size_t nthreads)
    : workers(),
      taskQueue(),
      activeTasks(0) {

    cout << "[ThreadPool] Creating pool with " << nthreads << " workers" << endl;
    for (size_t i = 0; i < nthreads; i++)
        this->workers.emplace_back([this] { this->workerLoop(); });
}

ThreadPool::~ThreadPool() {
    this->taskQueue.shutdown();  // Signal shutdown

    for (auto& worker : this->workers)
        if (worker.joinable())
            worker.join();

    cout << "[ThreadPool] All workers stopped" << endl;
}

void ThreadPool::enqueue(function<void()> task) {
    this->taskQueue.push(task);  // One line - beautiful!
}

bool ThreadPool::isIdle() const {
    return this->taskQueue.empty() && this->activeTasks == 0;
}

int ThreadPool::getQueueSize() const {
    return this->taskQueue.size();
}

void ThreadPool::workerLoop() {
    // Loop while tasks are available; pop() returns nullopt on shutdown
    while (auto taskOpt = this->taskQueue.pop()) {
        auto task = taskOpt.value();
        this->activeTasks++;
        task();
        this->activeTasks--;
    }
}
