#pragma once
#include <unordered_map>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>

/// <summary>
/// Singleton class that manages all textures in the game
/// Supports loading from asset lists and individual files
/// Separates base textures (loaded at startup) from streaming textures (loaded progressively)
/// </summary>
class TextureManager
{
public:
    typedef std::string String;
    typedef std::vector<sf::Texture*> TextureList;
    typedef std::unordered_map<String, TextureList> HashTable;

    static TextureManager* getInstance();
    
    // Loading methods
    void loadFromAssetList();  // Load all startup assets from a file
    void loadTexture(String assetName, String filePath);  // Load single texture manually
    void loadSingleStreamAsset(int index);  // Load a single streaming asset by directory index
    
    // Retrieval methods
    sf::Texture* getFromTextureMap(const String assetName, int frameIndex = 0);
    int getNumFrames(const String assetName);
    
    // Stream texture access (for IconObjects)
    sf::Texture* getStreamTextureFromList(const int index);
    int getNumLoadedStreamTextures() const;

private:
    TextureManager();
    TextureManager(TextureManager const&) {};
    TextureManager& operator=(TextureManager const&) {};
    static TextureManager* sharedInstance;

    HashTable textureMap;           // Maps name -> list of textures
    TextureList baseTextureList;    // Textures loaded at startup
    TextureList streamTextureList;  // Textures loaded progressively
    
    const std::string STREAMING_PATH = "Media/Streaming/";
    int streamingAssetCount = 0;
    
    // Helper methods
    void countStreamingAssets();
    void instantiateAsTexture(String path, String assetName, bool isStreaming);
};
