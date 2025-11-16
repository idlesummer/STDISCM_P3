#include "BGObject.h"
#include <iostream>
#include "TextureManager.h"
#include "BaseRunner.h"

BGObject::BGObject(String name) : 
    AGameObject(name)
{
}

void BGObject::initialize() {
    std::cout << "Declared as " << this->getName() << "\n";

    // Create sprite
    this->sprite = new sf::Sprite();
    
    // Get texture from TextureManager
    sf::Texture* texture = TextureManager::getInstance()->getFromTextureMap("Desert", 0);
    
    if (texture == nullptr) {
        std::cerr << "ERROR: Desert texture not loaded!" << std::endl;
        return;
    }
    
    // Enable texture repeating for infinite scroll effect
    texture->setRepeated(true);
    this->sprite->setTexture(*texture);
    
    // Make BG height × 8 to emulate repeating background
    // This creates a tall texture rectangle that we'll scroll through
    this->sprite->setTextureRect(sf::IntRect(0, 0, BaseRunner::WINDOW_WIDTH, BaseRunner::WINDOW_HEIGHT * 8)); 
    
    // Start position: way up so we can scroll down
    this->setPosition(0, -BaseRunner::WINDOW_HEIGHT * 7);
    
    std::cout << "Background initialized with repeating texture" << std::endl;
}

void BGObject::processInput(sf::Event event) {
    // Background doesn't need input
}

void BGObject::update(sf::Time deltaTime) {
    // Scroll the background downward
    sf::Vector2f position = this->getPosition();
    position.y += this->SPEED_MULTIPLIER * deltaTime.asSeconds();
    this->setPosition(position.x, position.y);

    // When background scrolls all the way down, reset to top
    // This creates an infinite scrolling effect
    if (position.y >= 0) 
        this->setPosition(0, -BaseRunner::WINDOW_HEIGHT * 7);
}
