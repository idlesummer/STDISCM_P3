#include "TextureManager.h"
#include "StringUtils.h"
#include <iostream>
#include <fstream>
#include <filesystem>

TextureManager* TextureManager::sharedInstance = NULL;

TextureManager* TextureManager::getInstance() 
{
    if (sharedInstance == NULL) 
    {
        sharedInstance = new TextureManager();
    }
    return sharedInstance;
}

TextureManager::TextureManager()
{
    std::cout << "[TextureManager] Initializing..." << std::endl;
    this->countStreamingAssets();
}

// ============================================
// LOADING METHODS
// ============================================

void TextureManager::loadFromAssetList()
{
    std::cout << "[TextureManager] Reading from asset list" << std::endl;
    std::ifstream stream("Media/assets.txt");
    
    if (!stream.is_open())
    {
        std::cerr << "[TextureManager] ERROR: Could not open Media/assets.txt" << std::endl;
        return;
    }
    
    String path;
    while (std::getline(stream, path))
    {
        // Parse filename from path
        std::vector<String> tokens = StringUtils::split(path, '/');
        String assetName = StringUtils::split(tokens[tokens.size() - 1], '.')[0];
        
        // Load as base texture (not streaming)
        this->instantiateAsTexture(path, assetName, false);
        std::cout << "[TextureManager] Loaded texture: " << assetName << std::endl;
    }
    
    stream.close();
}

void TextureManager::loadTexture(String assetName, String filePath)
{
    std::cout << "[TextureManager] Loading texture: " << assetName << " from " << filePath << std::endl;
    
    // Load as base texture (not streaming)
    this->instantiateAsTexture(filePath, assetName, false);
}

void TextureManager::loadSingleStreamAsset(int index)
{
    std::cout << "[TextureManager] Loading streaming asset at index " << index << std::endl;
    
    int fileNum = 0;
    
    for (const auto& entry : std::filesystem::directory_iterator(STREAMING_PATH))
    {
        if (index == fileNum)
        {
            String path = entry.path().string();
            
            // Parse filename from path (handle both / and \ separators)
            std::vector<String> pathTokens = StringUtils::split(path, '/');
            if (pathTokens.empty()) 
            {
                pathTokens = StringUtils::split(path, '\\');
            }
            
            // Extract asset name from filename
            String filename = pathTokens[pathTokens.size() - 1];
            std::vector<String> nameTokens = StringUtils::split(filename, '.');
            String assetName = nameTokens[0];
            
            // Load as streaming texture
            this->instantiateAsTexture(path, assetName, true);
            
            std::cout << "[TextureManager] Loaded streaming texture: " << assetName << std::endl;
            break;
        }
        
        fileNum++;
    }
}

// ============================================
// RETRIEVAL METHODS
// ============================================

sf::Texture* TextureManager::getFromTextureMap(const String assetName, int frameIndex)
{
    if (!this->textureMap[assetName].empty() && frameIndex < this->textureMap[assetName].size()) 
    {
        return this->textureMap[assetName][frameIndex];
    }
    else 
    {
        std::cout << "[TextureManager] No texture found for " << assetName << std::endl;
        return NULL;
    }
}

int TextureManager::getNumFrames(const String assetName)
{
    if (!this->textureMap[assetName].empty()) 
    {
        return this->textureMap[assetName].size();
    }
    else 
    {
        std::cout << "[TextureManager] No texture found for " << assetName << std::endl;
        return 0;
    }
}

sf::Texture* TextureManager::getStreamTextureFromList(const int index)
{
    if (index >= 0 && index < this->streamTextureList.size())
    {
        return this->streamTextureList[index];
    }
    else
    {
        std::cerr << "[TextureManager] Stream texture index out of bounds: " << index << std::endl;
        return NULL;
    }
}

int TextureManager::getNumLoadedStreamTextures() const
{
    return this->streamTextureList.size();
}

// ============================================
// PRIVATE HELPER METHODS
// ============================================

void TextureManager::countStreamingAssets()
{
    this->streamingAssetCount = 0;
    
    // Check if streaming directory exists
    if (!std::filesystem::exists(STREAMING_PATH))
    {
        std::cout << "[TextureManager] Streaming path does not exist: " << STREAMING_PATH << std::endl;
        return;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(STREAMING_PATH)) 
    {
        this->streamingAssetCount++;
    }
    
    std::cout << "[TextureManager] Number of streaming assets: " << this->streamingAssetCount << std::endl;
}

void TextureManager::instantiateAsTexture(String path, String assetName, bool isStreaming)
{
    sf::Texture* texture = new sf::Texture();
    
    if (!texture->loadFromFile(path))
    {
        std::cerr << "[TextureManager] ERROR: Failed to load " << path << std::endl;
        delete texture;
        return;
    }
    
    // Add to hash map
    this->textureMap[assetName].push_back(texture);
    
    // Add to appropriate list
    if (isStreaming)
    {
        this->streamTextureList.push_back(texture);
    }
    else
    {
        this->baseTextureList.push_back(texture);
    }
}
