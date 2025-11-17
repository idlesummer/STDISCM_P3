#pragma once
#include <thread>
#include <chrono>

class IETThread {
public:
    IETThread() = default;
    virtual ~IETThread() = default;

    void start() {
        std::thread(&IETThread::run, this).detach();
    }

    static void sleep(int ms) {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }

protected:
    virtual void run() = 0;
};
