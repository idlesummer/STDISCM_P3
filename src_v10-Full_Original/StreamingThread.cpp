#include "StreamingThread.h"
#include "TextureManager.h"
#include <iostream>

StreamingThread::StreamingThread(int textureIndex) : textureIndex(textureIndex) {
}

StreamingThread::~StreamingThread() {
}

void StreamingThread::run() {
    std::cout << "[StreamingThread] Loading texture " << textureIndex 
              << " on thread " << std::this_thread::get_id() << std::endl;
    
    // Load texture on this separate thread
    TextureManager::getInstance()->loadSingleStreamAsset(textureIndex);
    
    // Signal completion
    finished = true;
    
    std::cout << "[StreamingThread] Finished loading texture " << textureIndex << std::endl;
}

bool StreamingThread::isFinished() const {
    return finished.load();  // Thread-safe read
}
