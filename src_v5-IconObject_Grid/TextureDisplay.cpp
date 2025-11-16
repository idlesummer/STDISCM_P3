#include "TextureDisplay.h"
#include "IconObject.h"
#include "TextureManager.h"
#include "GameObjectManager.h"
#include "BaseRunner.h"
#include <iostream>

TextureDisplay::TextureDisplay() : AGameObject("TextureDisplay")
{
}

TextureDisplay::~TextureDisplay() {
    // IconObjects are managed by GameObjectManager
}

void TextureDisplay::initialize() {
    std::cout << "TextureDisplay initialized" << std::endl;
    std::cout << "Grid: " << MAX_COLUMN << " columns × " << MAX_ROW << " rows" << std::endl;
    std::cout << "Icon size: " << IMG_WIDTH << "×" << IMG_HEIGHT << " pixels" << std::endl;
}

void TextureDisplay::processInput(sf::Event event) {
    // No input handling needed
}

void TextureDisplay::update(sf::Time deltaTime) {
    // Accumulate time
    this->ticks += BaseRunner::TIME_PER_FRAME.asMilliseconds();

    // Every STREAMING_LOAD_DELAY milliseconds (1 second)
    if (this->ticks >= this->STREAMING_LOAD_DELAY) {
        this->ticks = 0.0f;

        int currentIndex = this->iconList.size();
        
        // Check if we still have textures to load
        // For this demo, let's load up to 50 icons (you can increase this)
        const int MAX_ICONS = 50;
        
        if (currentIndex < MAX_ICONS) {
            std::cout << "[TextureDisplay] Loading streaming texture at index " 
                      << currentIndex << std::endl;
            
            // Load one streaming texture
            TextureManager::getInstance()->loadSingleStreamAsset(currentIndex);
            
            // Spawn an IconObject to display it
            this->spawnObject();
            
            std::cout << "[TextureDisplay] Icons spawned: " << this->iconList.size() 
                      << "/" << MAX_ICONS << std::endl;
        }
    }
    /*
    Better structure of code below:

    void TextureDisplay::update(sf::Time deltaTime) {
        // Accumulate time
        this->ticks += BaseRunner::TIME_PER_FRAME.asMilliseconds();

        // Check if enough time passed?
        if (this->ticks < this->STREAMING_LOAD_DELAY)
            return;

        // Time has passed! Reset timer
        this->ticks = 0.0f;

        // GUARD 2: Have we reached the limit?
        const int MAX_ICONS = 50;
        int currentIndex = this->iconList.size();

        // Check if already loaded enough, exit early
        if (currentIndex >= MAX_ICONS)
            return;

        // All checks passed! Load and spawn
        std::cout << "[TextureDisplay] Loading streaming texture at index " 
                << currentIndex << std::endl;

        TextureManager::getInstance()->loadSingleStreamAsset(currentIndex);
        this->spawnObject();

        std::cout << "[TextureDisplay] Icons spawned: " << this->iconList.size() 
                << "/" << MAX_ICONS << std::endl;
    }
    */
}

void TextureDisplay::spawnObject() {
    // Create unique name for this icon
    String objectName = "Icon_" + std::to_string(this->iconList.size());
    
    // Create IconObject with the texture index
    IconObject* iconObj = new IconObject(objectName, this->iconList.size());
    this->iconList.push_back(iconObj);

    // Calculate grid position
    float x = this->columnGrid * IMG_WIDTH;
    float y = this->rowGrid * IMG_HEIGHT;
    iconObj->setPosition(x, y);

    std::cout << "Spawned " << objectName << " at position (" << x << ", " << y << ")" << std::endl;

    // Move to next grid cell
    this->columnGrid++;
    if (this->columnGrid >= this->MAX_COLUMN) {
        this->columnGrid = 0;  // Reset to start of row
        this->rowGrid++;       // Move to next row
    }
    
    // Add to game object manager (calls initialize and starts updating/drawing)
    GameObjectManager::getInstance()->addObject(iconObj);
}
