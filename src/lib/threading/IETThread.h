#pragma once
#include <thread>

class IETThread {
public:
    IETThread();
    ~IETThread();
    
    void start();
    static void sleep(int ms);

protected:
    virtual void run() = 0;
};
