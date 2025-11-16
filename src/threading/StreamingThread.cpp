#include "StreamingThread.h"
#include "../managers/TextureManager.h"

using namespace std;


StreamingThread::StreamingThread(int index) 
    : textureIndex(index),
      finished(false) {
}

StreamingThread::~StreamingThread() {
    // no-op
}

void StreamingThread::run() {
    TextureManager::getInstance()->loadSingleStreamAsset(this->textureIndex);
    this->finished = true;
}

bool StreamingThread::isFinished() const {
    return this->finished.load();
}
