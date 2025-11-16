#pragma once
#include "lib/threading/IETThread.h"
#include <atomic>

class StreamingThread : public IETThread {
public:
    StreamingThread(int index);
    ~StreamingThread();
    
    bool isFinished() const;

protected:
    void run() override;

private:
    int textureIndex;
    std::atomic<bool> finished;
};
