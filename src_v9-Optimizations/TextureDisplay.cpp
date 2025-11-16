#include "TextureDisplay.h"
#include "IconObject.h"
#include "TextureManager.h"
#include "GameObjectManager.h"
#include "BaseRunner.h"
#include "StreamingThread.h"
#include <iostream>

TextureDisplay::TextureDisplay() : AGameObject("TextureDisplay") {
}

TextureDisplay::~TextureDisplay() {
    // IconObjects managed by GameObjectManager
}

void TextureDisplay::initialize() {
    std::cout << "TextureDisplay initialized (Thread Pool Mode)" << std::endl;
    std::cout << "Grid: " << MAX_COLUMN << " columns × " << MAX_ROW << " rows" << std::endl;
    std::cout << "Icon size: " << IMG_WIDTH << "×" << IMG_HEIGHT << " pixels" << std::endl;
    std::cout << "Textures will load in PARALLEL using thread pool!" << std::endl;
}

void TextureDisplay::processInput(sf::Event event) {
    // No input handling needed
}

void TextureDisplay::update(sf::Time deltaTime) {
    // Accumulate time
    ticks += BaseRunner::TIME_PER_FRAME.asMilliseconds();

    // Schedule batch loading at intervals
    if (ticks >= STREAMING_LOAD_DELAY) {
        ticks = 0.0f;

        int currentIndex = iconList.size();
        int batchSize = 20;  // Load 20 at a time
        const int MAX_ICONS = 50;

        if (currentIndex < MAX_ICONS) {
            std::cout << "[TextureDisplay] Scheduling batch load starting at index "
                      << currentIndex << std::endl;
            TextureManager::getInstance()->loadBatchAsync(currentIndex, batchSize);
        }
    }

    // Check ready queue and spawn objects
    while (TextureManager::getInstance()->hasReadyTexture()) {
        TextureManager::LoadedTexture loaded = TextureManager::getInstance()->popReadyTexture();
        
        std::cout << "[TextureDisplay] Spawning icon for texture " << loaded.index << std::endl;
        spawnObject();
        
        std::cout << "[TextureDisplay] Icons spawned: " << iconList.size() 
                  << "/50 (Ready queue: " 
                  << TextureManager::getInstance()->getReadyQueueSize() << ")" << std::endl;
    }
}

void TextureDisplay::spawnObject() {
    // Create unique name for this icon
    String objectName = "Icon_" + std::to_string(iconList.size());
    
    // Create IconObject with the texture index
    IconObject* iconObj = new IconObject(objectName, iconList.size());
    iconList.push_back(iconObj);

    // Calculate grid position
    float x = columnGrid * IMG_WIDTH;
    float y = rowGrid * IMG_HEIGHT;
    iconObj->setPosition(x, y);

    std::cout << "Spawned " << objectName << " at position (" << x << ", " << y << ")" << std::endl;

    // Move to next grid cell
    columnGrid++;
    if (columnGrid >= MAX_COLUMN) {
        columnGrid = 0;  // Reset to start of row
        rowGrid++;       // Move to next row
    }
    
    // Add to game object manager (calls initialize and starts updating/drawing)
    GameObjectManager::getInstance()->addObject(iconObj);
}
