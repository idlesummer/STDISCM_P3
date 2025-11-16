#pragma once
#include <unordered_map>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>

/// <summary>
/// Singleton class that manages all textures in the game
/// Prevents loading the same texture multiple times
/// </summary>
class TextureManager
{
public:
    typedef std::string String;
    typedef std::vector<sf::Texture*> TextureList;
    typedef std::unordered_map<String, TextureList> HashTable;

    static TextureManager* getInstance();
    
    // Load a single texture and store it with a name
    void loadTexture(String assetName, String filePath);
    
    // Get a texture by name (and optionally frame index for animations)
    sf::Texture* getFromTextureMap(const String assetName, int frameIndex = 0);
    
    // Get number of frames for an animated texture
    int getNumFrames(const String assetName);

private:
    TextureManager() {};
    TextureManager(TextureManager const&) {};
    TextureManager& operator=(TextureManager const&) {};
    static TextureManager* sharedInstance;

    HashTable textureMap;  // Maps name -> list of textures (for animation support)
};
