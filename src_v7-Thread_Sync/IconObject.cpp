#include "IconObject.h"
#include <iostream>
#include "TextureManager.h"

IconObject::IconObject(String name, int textureIndex) : 
    AGameObject(name) 
{
    this->textureIndex = textureIndex;
}

void IconObject::initialize() {
    std::cout << "IconObject " << this->getName() << " initialized with texture index " 
              << this->textureIndex << std::endl;
    
    // Get texture from stream texture list by index
    sf::Texture* texture = TextureManager::getInstance()->getStreamTextureFromList(this->textureIndex);
    
    if (texture == nullptr) {
        std::cerr << "ERROR: Stream texture at index " << this->textureIndex << " not found!" << std::endl;
        return;
    }
    
    // Create sprite and assign texture
    this->sprite = new sf::Sprite();
    this->sprite->setTexture(*texture);
}

void IconObject::processInput(sf::Event event) {
    // Icons don't need input handling
}

void IconObject::update(sf::Time deltaTime) {
    // Icons are static (don't move)
}
