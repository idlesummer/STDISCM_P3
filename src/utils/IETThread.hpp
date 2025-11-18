#pragma once
#include <thread>
#include <chrono>

using namespace std;


class IETThread {
public:
  IETThread() = default;
  virtual ~IETThread() = default;

  void start() {
    thread(&IETThread::run, this).detach();
  }

  static void sleep(int ms) {
    this_thread::sleep_for(chrono::milliseconds(ms));
  }

protected:
  virtual void run() = 0;
};
