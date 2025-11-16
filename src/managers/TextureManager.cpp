#include "TextureManager.h"
#include "../utils/StringUtils.h"
#include "../threading/IETThread.h"
#include "../threading/ThreadPool.h"
#include <iostream>
#include <filesystem>

using namespace std;
using namespace sf;


// ===== STATIC MEMBER INITIALIZATION ===== 
TextureManager* TextureManager::sharedInstance = nullptr;


// ===== CLASS IMPLEMENTATION ===== 
TextureManager::TextureManager()
    : textureMap(),
      baseTextureList(),
      streamTextureList(),
      STREAMING_PATH("assets/Streaming/"),
      streamingAssetCount(0),
      streamingAssetsCounted(false),
      streamingFilePaths(),
      readyQueue(),
      queueMutex(),
      threadPool(nullptr),
      initialized(false),
      initMutex(),
      initCV() {

    cout << "[TextureManager] Starting background initialization..." << endl;

    // Start background initialization thread to avoid blocking main thread
    thread([this]() {
        cout << "[TextureManager] Background: Creating thread pool..." << endl;
        this->threadPool = new ThreadPool(thread::hardware_concurrency());

        cout << "[TextureManager] Background: Counting streaming assets..." << endl;
        this->countStreamingAssets();
        this->streamingAssetsCounted = true;

        // Signal that initialization is complete
        {
            auto lock = lock_guard<mutex>(this->initMutex);
            this->initialized = true;
        }
        this->initCV.notify_all();
        cout << "[TextureManager] Background initialization complete!" << endl;
    }).detach();
}

TextureManager::~TextureManager() {
    delete this->threadPool;
}

void TextureManager::waitForInitialization() {
    // Fast path: lock-free check if already initialized
    if (this->initialized.load())
        return;

    // Slow path: wait for initialization to complete
    auto lock = unique_lock<mutex>(this->initMutex);
    this->initCV.wait(lock, [this] { return this->initialized.load(); });
}

void TextureManager::ensureThreadPoolCreated() {
    // Legacy method - now handled by background initialization
    this->waitForInitialization();
}

void TextureManager::ensureStreamingAssetsCounted() {
    // Legacy method - now handled by background initialization
    this->waitForInitialization();
}

TextureManager* TextureManager::getInstance() {
    if (sharedInstance == nullptr)
        sharedInstance = new TextureManager();
    return sharedInstance;
}

void TextureManager::loadTexture(const String& assetName, const String& filePath) {
    this->instantiateAsTexture(filePath, assetName, false);
}

void TextureManager::loadSingleStreamAsset(int index) {
    this->loadSingleStreamAssetSync(index);
}

void TextureManager::loadBatchAsync(int startIndex, int count) {
    this->ensureThreadPoolCreated();
    this->ensureStreamingAssetsCounted();

    cout << "[TextureManager] Scheduling batch load: " << count
         << " textures starting at index " << startIndex << endl;

    for (auto i = 0; i < count; i++) {
        auto index = startIndex + i;

        if (index >= this->streamingAssetCount)
            break;

        this->threadPool->enqueue([this, index]() {
            this->loadSingleStreamAssetSync(index);
        });
    }
}

void TextureManager::loadSingleStreamAssetSync(int index) {
    // Bounds check
    if (index < 0 || index >= static_cast<int>(this->streamingFilePaths.size())) {
        cerr << "[TextureManager] ERROR: Index " << index << " out of bounds" << endl;
        return;
    }

    // O(1) access to cached file path
    auto pathStr = this->streamingFilePaths[index];
    auto path = filesystem::path(pathStr);
    auto assetName = path.stem().string();  // Get filename without extension

    IETThread::sleep(100);

    auto texture = new Texture();
    if (!texture->loadFromFile(pathStr)) {
        cerr << "[TextureManager] ERROR: Failed to load " << pathStr << endl;
        delete texture;
        return;
    }

    this->textureMap[assetName].push_back(texture);
    this->streamTextureList.push_back(texture);

    auto loaded = LoadedTexture();
    loaded.texture = texture;
    loaded.assetName = assetName;
    loaded.index = index;

    this->addToReadyQueue(loaded);
}

Texture* TextureManager::getFromTextureMap(const String& assetName, int frameIndex) {
    if (this->textureMap[assetName].empty() || frameIndex >= this->textureMap[assetName].size()) {
        return nullptr;
    }

    return this->textureMap[assetName][frameIndex];
}

int TextureManager::getNumFrames(const String& assetName) {
    if (this->textureMap[assetName].empty()) {
        return 0;
    }

    return static_cast<int>(this->textureMap[assetName].size());
}

Texture* TextureManager::getStreamTextureFromList(const int index) {
    if (index < 0 || index >= static_cast<int>(this->streamTextureList.size())) {
        cerr << "[TextureManager] Stream texture index out of bounds: " << index << endl;
        return nullptr;
    }

    return this->streamTextureList[index];
}

int TextureManager::getNumLoadedStreamTextures() const {
    return static_cast<int>(this->streamTextureList.size());
}

void TextureManager::countStreamingAssets() {
    this->streamingAssetCount = 0;
    this->streamingFilePaths.clear();

    if (!filesystem::exists(this->STREAMING_PATH)) {
        cout << "[TextureManager] Streaming path does not exist: " << this->STREAMING_PATH << endl;
        return;
    }

    // Cache file paths for O(1) access by index
    for (const auto& entry : filesystem::directory_iterator(this->STREAMING_PATH)) {
        this->streamingFilePaths.push_back(entry.path().string());
        this->streamingAssetCount++;
    }

    cout << "[TextureManager] Number of streaming assets: " << this->streamingAssetCount << endl;
    cout << "[TextureManager] Cached " << this->streamingFilePaths.size() << " file paths" << endl;
}

void TextureManager::instantiateAsTexture(const String& path, const String& assetName, bool isStreaming) {
    auto texture = new Texture();

    if (!texture->loadFromFile(path)) {
        cerr << "[TextureManager] ERROR: Failed to load " << path << endl;
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
    auto lock = lock_guard<mutex>(this->queueMutex);
    this->readyQueue.push(loaded);
    // cout removed from hot path - called 480+ times
}

bool TextureManager::hasReadyTexture() {
    auto lock = lock_guard<mutex>(this->queueMutex);
    return !this->readyQueue.empty();
}

TextureManager::LoadedTexture TextureManager::popReadyTexture() {
    auto lock = lock_guard<mutex>(this->queueMutex);
    auto loaded = this->readyQueue.front();
    this->readyQueue.pop();
    // cout removed from hot path - called 480+ times
    return loaded;
}

int TextureManager::getReadyQueueSize() const {
    return static_cast<int>(this->readyQueue.size());
}
