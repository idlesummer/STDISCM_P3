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
    // Clean up active thread if exists
    if (currentStreamThread != nullptr) {
        delete currentStreamThread;
        currentStreamThread = nullptr;
    }
}

void TextureDisplay::initialize() {
    std::cout << "TextureDisplay initialized (Threading Mode)" << std::endl;
    std::cout << "Grid: " << MAX_COLUMN << " columns × " << MAX_ROW << " rows" << std::endl;
    std::cout << "Icon size: " << IMG_WIDTH << "×" << IMG_HEIGHT << " pixels" << std::endl;
    std::cout << "Textures will load on SEPARATE THREADS!" << std::endl;
}

void TextureDisplay::processInput(sf::Event event) {
    // No input handling needed
}

void TextureDisplay::update(sf::Time deltaTime) {
    // Accumulate time
    ticks += BaseRunner::TIME_PER_FRAME.asMilliseconds();

    // Check if we should start a new load
    if (ticks >= STREAMING_LOAD_DELAY && currentStreamThread == nullptr) {
        ticks = 0.0f;

        int currentIndex = iconList.size();
        const int MAX_ICONS = 50;
        
        if (currentIndex < MAX_ICONS) {
            std::cout << "[TextureDisplay] Starting thread to load texture " 
                      << currentIndex << std::endl;
            
            // Create and start worker thread
            currentStreamThread = new StreamingThread(currentIndex);
            currentStreamThread->start();  // Runs on separate thread!
        }
    }

    // Check if worker thread finished
    if (currentStreamThread != nullptr && currentStreamThread->isFinished()) {
        std::cout << "[TextureDisplay] Thread finished! Spawning icon..." << std::endl;
        
        // Spawn icon for the loaded texture
        spawnObject();
        
        // Clean up finished thread
        delete currentStreamThread;
        currentStreamThread = nullptr;
        
        std::cout << "[TextureDisplay] Icons spawned: " << iconList.size() 
                  << "/50" << std::endl;
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
