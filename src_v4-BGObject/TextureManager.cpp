#include "TextureManager.h"
#include <iostream>

TextureManager* TextureManager::sharedInstance = NULL;

TextureManager* TextureManager::getInstance() {
    if (sharedInstance == NULL) 
        sharedInstance = new TextureManager();
    return sharedInstance;
}

void TextureManager::loadTexture(String assetName, String filePath) {
    std::cout << "[TextureManager] Loading texture: " << assetName << " from " << filePath << std::endl;
    
    sf::Texture* texture = new sf::Texture();
    
    if (!texture->loadFromFile(filePath)) {
        std::cerr << "[TextureManager] ERROR: Failed to load " << filePath << std::endl;
        delete texture;
        return;
    }
    
    // Store in map (as a vector to support animation frames later)
    this->textureMap[assetName].push_back(texture);
    
    std::cout << "[TextureManager] Successfully loaded: " << assetName << std::endl;
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
