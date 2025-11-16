#include <chrono>
#include "IETThread.h"

using namespace std;


IETThread::IETThread() {
    // no-op
}

IETThread::~IETThread() {
    // no-op
}

void IETThread::start() {
    thread(&IETThread::run, this).detach();
}

void IETThread::sleep(int ms) {
    this_thread::sleep_for(chrono::milliseconds(ms));
}
