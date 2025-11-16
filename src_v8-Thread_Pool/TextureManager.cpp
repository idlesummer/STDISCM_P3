#include "TextureManager.h"
#include "StringUtils.h"
#include "IETThread.h"
#include "ThreadPool.h"
#include <iostream>
#include <filesystem>

TextureManager* TextureManager::sharedInstance = NULL;

TextureManager* TextureManager::getInstance() {
    if (sharedInstance == NULL)
        sharedInstance = new TextureManager();
    return sharedInstance;
}

TextureManager::TextureManager() {
    std::cout << "[TextureManager] Initializing with thread pool..." << std::endl;
    this->threadPool = new ThreadPool(4);  // 4 worker threads
    this->countStreamingAssets();
}

TextureManager::~TextureManager() {
    delete threadPool;
}

void TextureManager::loadTexture(String assetName, String filePath) {
    std::cout << "[TextureManager] Loading texture: " << assetName << " from " << filePath << std::endl;
    this->instantiateAsTexture(filePath, assetName, false);
}

void TextureManager::loadSingleStreamAsset(int index) {
    this->loadSingleStreamAssetSync(index);
}

void TextureManager::loadBatchAsync(int startIndex, int count) {
    std::cout << "[TextureManager] Scheduling batch load: " << count 
              << " textures starting at index " << startIndex << std::endl;
    
    for (int i = 0; i < count; i++) {
        int index = startIndex + i;
        if (index >= this->streamingAssetCount)
            break;
        
        threadPool->enqueueTask([this, index]() {
            this->loadSingleStreamAssetSync(index);
        });
    }
}

void TextureManager::loadSingleStreamAssetSync(int index) {
    int fileNum = 0;
    
    for (const auto& entry : std::filesystem::directory_iterator(STREAMING_PATH)) {
        if (index == fileNum) {
            IETThread::sleep(100);

            String path = entry.path().string();

            std::vector<String> pathTokens = StringUtils::split(path, '/');
            if (pathTokens.empty())
                pathTokens = StringUtils::split(path, '\\');
            
            String filename = pathTokens[pathTokens.size() - 1];
            std::vector<String> nameTokens = StringUtils::split(filename, '.');
            String assetName = nameTokens[0];

            sf::Texture* texture = new sf::Texture();
            if (!texture->loadFromFile(path)) {
                std::cerr << "[TextureManager] ERROR: Failed to load " << path << std::endl;
                delete texture;
                break;
            }

            this->textureMap[assetName].push_back(texture);
            this->streamTextureList.push_back(texture);

            LoadedTexture loaded;
            loaded.texture = texture;
            loaded.assetName = assetName;
            loaded.index = index;

            this->addToReadyQueue(loaded);

            std::cout << "[TextureManager] Loaded streaming texture: " << assetName << std::endl;
            break;
        }
        fileNum++;
    }
}

sf::Texture* TextureManager::getFromTextureMap(const String assetName, int frameIndex) {
    if (!this->textureMap[assetName].empty() && frameIndex < this->textureMap[assetName].size())
        return this->textureMap[assetName][frameIndex];
    else {
        std::cout << "[TextureManager] No texture found for " << assetName << std::endl;
        return NULL;
    }
}

int TextureManager::getNumFrames(const String assetName) {
    if (!this->textureMap[assetName].empty())
        return this->textureMap[assetName].size();
    else {
        std::cout << "[TextureManager] No texture found for " << assetName << std::endl;
        return 0;
    }
}

sf::Texture* TextureManager::getStreamTextureFromList(const int index) {
    if (index >= 0 && index < this->streamTextureList.size())
        return this->streamTextureList[index];
    else {
        std::cerr << "[TextureManager] Stream texture index out of bounds: " << index << std::endl;
        return NULL;
    }
}

int TextureManager::getNumLoadedStreamTextures() const {
    return this->streamTextureList.size();
}

void TextureManager::countStreamingAssets() {
    this->streamingAssetCount = 0;
    
    if (!std::filesystem::exists(STREAMING_PATH)) {
        std::cout << "[TextureManager] Streaming path does not exist: " << STREAMING_PATH << std::endl;
        return;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(STREAMING_PATH))
        this->streamingAssetCount++;
    
    std::cout << "[TextureManager] Number of streaming assets: " << this->streamingAssetCount << std::endl;
}

void TextureManager::instantiateAsTexture(String path, String assetName, bool isStreaming) {
    sf::Texture* texture = new sf::Texture();
    
    if (!texture->loadFromFile(path)) {
        std::cerr << "[TextureManager] ERROR: Failed to load " << path << std::endl;
        delete texture;
        return;
    }
    
    this->textureMap[assetName].push_back(texture);
    
    if (isStreaming)
        this->streamTextureList.push_back(texture);
    else
        this->baseTextureList.push_back(texture);
}

void TextureManager::addToReadyQueue(LoadedTexture loaded) {
    std::lock_guard<std::mutex> lock(queueMutex);
    readyQueue.push(loaded);
    std::cout << "[TextureManager] Added to ready queue on thread " 
              << std::this_thread::get_id() << " (Queue size: " 
              << readyQueue.size() << ")" << std::endl;
}

bool TextureManager::hasReadyTexture() {
    std::lock_guard<std::mutex> lock(queueMutex);
    return !readyQueue.empty();
}

TextureManager::LoadedTexture TextureManager::popReadyTexture() {
    std::lock_guard<std::mutex> lock(queueMutex);
    LoadedTexture loaded = readyQueue.front();
    readyQueue.pop();
    std::cout << "[TextureManager] Popped from ready queue on thread " 
              << std::this_thread::get_id() << " (Remaining: " 
              << readyQueue.size() << ")" << std::endl;
    return loaded;
}

int TextureManager::getReadyQueueSize() const {
    return readyQueue.size();
}
