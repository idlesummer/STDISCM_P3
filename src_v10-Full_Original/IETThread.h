#pragma once
#include <thread>
#include <chrono>

/// <summary>
/// Abstract thread class inspired by Java/C# threading model
/// Inherit from this class and implement run() method
/// Call start() to begin thread execution
/// By: NeilDG
/// </summary>
class IETThread {
public:
    IETThread();
    virtual ~IETThread();

    void start();  // Schedules thread execution
    static void sleep(int ms);  // Sleep for milliseconds

protected:
    virtual void run() = 0;  // Override this with thread behavior
};
