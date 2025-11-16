#pragma once
#include "IETThread.h"
#include <atomic>

/// <summary>
/// Thread responsible for loading a single streaming texture
/// Uses atomic flag to signal completion
/// </summary>
class StreamingThread : public IETThread {
public:
    StreamingThread(int textureIndex);
    ~StreamingThread();

    bool isFinished() const;  // Check if loading is complete

protected:
    void run() override;  // Loads the texture on separate thread

private:
    int textureIndex = 0;
    std::atomic<bool> finished{false};  // Thread-safe flag
};
