#include "ThreadPool.h"
#include <iostream>

using namespace std;


ThreadPool::ThreadPool(size_t numThreads)
    : workers(),
      taskQueue(),
      activeTasks(0) {

    cout << "[ThreadPool] Creating pool with " << numThreads << " workers" << endl;
    for (size_t i = 0; i < numThreads; i++)
        this->workers.emplace_back([this] { this->workerThread(); });
}

ThreadPool::~ThreadPool() {
    this->taskQueue.shutdown();  // Signal shutdown - clean!

    for (auto& worker : this->workers) {
        if (worker.joinable())
            worker.join();
    }

    cout << "[ThreadPool] All workers stopped" << endl;
}

void ThreadPool::enqueueTask(function<void()> task) {
    this->taskQueue.push(task);  // One line - beautiful!
}

bool ThreadPool::isIdle() const {
    return this->taskQueue.empty() && this->activeTasks == 0;
}

int ThreadPool::getQueueSize() const {
    return this->taskQueue.size();
}

void ThreadPool::workerThread() {
    cout << "[ThreadPool] Worker " << this_thread::get_id() << " started" << endl;

    while (true) {
        // Blocking pop - waits for task or shutdown
        auto taskOpt = this->taskQueue.pop();

        // If nullopt, queue is shutdown and empty
        if (!taskOpt.has_value())
            return;

        auto task = taskOpt.value();
        this->activeTasks++;

        cout << "[ThreadPool] Worker " << this_thread::get_id();
        cout << " executing task (Active: " << this->activeTasks << ")" << endl;
        task();

        cout << "[ThreadPool] Worker " << this_thread::get_id() << " finished task" << endl;
        this->activeTasks--;
    }
}
