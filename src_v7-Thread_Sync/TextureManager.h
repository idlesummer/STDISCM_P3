#pragma once
#include <unordered_map>
#include <vector>
#include <queue>
#include <mutex>
#include <string>
#include <SFML/Graphics.hpp>

class TextureManager {
public:
    typedef std::string String;
    typedef std::vector<sf::Texture*> TextureList;
    typedef std::unordered_map<String, TextureList> HashTable;

    // Structure for loaded texture info
    struct LoadedTexture {
        sf::Texture* texture;
        String assetName;
        int index;
    };

public:
    static TextureManager* getInstance();
    void loadTexture(String assetName, String filePath);
    void loadSingleStreamAsset(int index);
    sf::Texture* getFromTextureMap(const String assetName, int frameIndex = 0);
    int getNumFrames(const String assetName);
    
    sf::Texture* getStreamTextureFromList(const int index);
    int getNumLoadedStreamTextures() const;

    // Thread-safe queue operations (NEW in v7!)
    void addToReadyQueue(LoadedTexture loaded);
    bool hasReadyTexture();
    LoadedTexture popReadyTexture();
    int getReadyQueueSize() const;

private:
    TextureManager();
    TextureManager(TextureManager const&) {}
    TextureManager& operator=(TextureManager const&) { return *this; }
    static TextureManager* sharedInstance;

    HashTable textureMap;
    TextureList baseTextureList;
    TextureList streamTextureList;

    const std::string STREAMING_PATH = "Media/Streaming/";
    int streamingAssetCount = 0;

    void countStreamingAssets();
    void instantiateAsTexture(String path, String assetName, bool isStreaming);

    // Thread synchronization (NEW in v7!)
    std::queue<LoadedTexture> readyQueue;
    std::mutex queueMutex;
};
