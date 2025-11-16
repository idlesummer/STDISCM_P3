#include "ThreadPool.h"
#include <iostream>

ThreadPool::ThreadPool(size_t numThreads) : stop(false), activeTasks(0) {
    std::cout << "[ThreadPool] Creating pool with " << numThreads << " workers" << std::endl;
    
    for (size_t i = 0; i < numThreads; i++)
        workers.emplace_back([this] { this->workerThread(); });
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stop = true;
    }
    condition.notify_all();

    for (std::thread& worker : workers) {
        if (worker.joinable())
            worker.join();
    }
    
    std::cout << "[ThreadPool] All workers stopped" << std::endl;
}

void ThreadPool::enqueueTask(std::function<void()> task) {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        tasks.push(task);
    }
    condition.notify_one();
}

bool ThreadPool::isIdle() const {
    return tasks.empty() && activeTasks == 0;
}

int ThreadPool::getQueueSize() const {
    return tasks.size();
}

void ThreadPool::workerThread() {
    std::cout << "[ThreadPool] Worker " << std::this_thread::get_id() << " started" << std::endl;
    
    while (true) {
        std::function<void()> task;

        {
            std::unique_lock<std::mutex> lock(queueMutex);
            condition.wait(lock, [this] { return stop || !tasks.empty(); });

            if (stop && tasks.empty())
                return;

            task = std::move(tasks.front());
            tasks.pop();
            activeTasks++;
        }

        std::cout << "[ThreadPool] Worker " << std::this_thread::get_id() 
                  << " executing task (Active: " << activeTasks << ")" << std::endl;
        task();
        std::cout << "[ThreadPool] Worker " << std::this_thread::get_id() 
                  << " finished task" << std::endl;

        activeTasks--;
    }
}
