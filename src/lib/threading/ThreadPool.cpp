#include "lib/threading/ThreadPool.h"
#include <iostream>

using namespace std;


ThreadPool::ThreadPool(size_t numThreads) 
    : workers(),
      tasks(),
      queueMutex(),
      condition(),
      stop(false),
      activeTasks(0) {

    cout << "[ThreadPool] Creating pool with " << numThreads << " workers" << endl;
    for (size_t i = 0; i < numThreads; i++)
        this->workers.emplace_back([this] { this->workerThread(); });
}

ThreadPool::~ThreadPool() {
    {
        auto lock = lock_guard<mutex>(this->queueMutex);
        this->stop = true;
    }
    this->condition.notify_all();
    
    for (auto& worker : this->workers) {
        if (worker.joinable())
            worker.join();
    }
    
    cout << "[ThreadPool] All workers stopped" << endl;
}

void ThreadPool::enqueueTask(function<void()> task) {
    {
        auto lock = lock_guard<mutex>(this->queueMutex);
        this->tasks.push(task);
    }
    this->condition.notify_one();
}

bool ThreadPool::isIdle() const {
    return this->tasks.empty() && this->activeTasks == 0;
}

int ThreadPool::getQueueSize() const {
    return this->tasks.size();
}

void ThreadPool::workerThread() {
    cout << "[ThreadPool] Worker " << this_thread::get_id() << " started" << endl;
    
    while (true) {
        auto task = function<void()>();
        {
            auto lock = unique_lock<mutex>(this->queueMutex);
            this->condition.wait(lock, [this] { return this->stop || !this->tasks.empty(); });
            
            if (this->stop && this->tasks.empty())
                return;
            
            task = move(this->tasks.front());
            this->tasks.pop();
            this->activeTasks++;
        }
        
        cout << "[ThreadPool] Worker " << this_thread::get_id();
        cout << " executing task (Active: " << this->activeTasks << ")" << endl;
        task();
        
        cout << "[ThreadPool] Worker " << this_thread::get_id() << " finished task" << endl;
        this->activeTasks--;
    }
}
