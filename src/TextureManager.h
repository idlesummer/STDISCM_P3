#pragma once
#include <unordered_map>
#include <vector>
#include <queue>
#include <mutex>
#include <string>
#include <SFML/Graphics.hpp>

class ThreadPool;

class TextureManager {
public:
    using String = std::string;
    using TextureList = std::vector<sf::Texture*>;
    using HashTable = std::unordered_map<String, TextureList>;
    
    struct LoadedTexture {
        sf::Texture* texture;
        String assetName;
        int index;
    };

public:
    static TextureManager* getInstance();
    
    void loadTexture(String assetName, String filePath);
    void loadSingleStreamAsset(int index);
    void loadBatchAsync(int startIndex, int count);
    
    sf::Texture* getFromTextureMap(const String assetName, int frameIndex = 0);
    int getNumFrames(const String assetName);
    sf::Texture* getStreamTextureFromList(const int index);
    int getNumLoadedStreamTextures() const;
    
    void addToReadyQueue(LoadedTexture loaded);
    bool hasReadyTexture();
    LoadedTexture popReadyTexture();
    int getReadyQueueSize() const;

private:
    TextureManager();
    ~TextureManager();
    TextureManager(TextureManager const&) {}
    TextureManager& operator=(TextureManager const&) { return *this; }
    
    static TextureManager* sharedInstance;
    
    HashTable textureMap;
    TextureList baseTextureList;
    TextureList streamTextureList;
    std::string STREAMING_PATH;
    int streamingAssetCount;
    std::queue<LoadedTexture> readyQueue;
    std::mutex queueMutex;
    ThreadPool* threadPool;
    
    void countStreamingAssets();
    void instantiateAsTexture(String path, String assetName, bool isStreaming);
    void loadSingleStreamAssetSync(int index);
};
