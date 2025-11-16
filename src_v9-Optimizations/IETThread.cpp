#include "IETThread.h"

IETThread::IETThread() {
}

IETThread::~IETThread() {
}

void IETThread::start() {
    // Create thread and detach (fire and forget)
    std::thread(&IETThread::run, this).detach();
}

void IETThread::sleep(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}
