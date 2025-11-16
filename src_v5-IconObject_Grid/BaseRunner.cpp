#include "BaseRunner.h"
#include "GameObjectManager.h"
#include "BGObject.h"
#include "TextureManager.h"
#include "TextureDisplay.h"
#include "SimpleFPSCounter.h"
#include <iostream>

const sf::Time BaseRunner::TIME_PER_FRAME = sf::seconds(1.f / 60.f);

BaseRunner::BaseRunner() :
    window(
        sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), 
        "Minimal v5: Icon Grid + Progressive Loading", 
        sf::Style::Close
    ) 
{
    std::cout << "=== Minimal v5: Icon Grid + Progressive Loading ===" << std::endl;
    std::cout << std::endl;
    
    // STEP 1: Load base textures using TextureManager
    std::cout << "Loading base textures..." << std::endl;
    TextureManager::getInstance()->loadTexture("Desert", "Media/Textures/Desert.png");
    std::cout << std::endl;
    
    // STEP 2: Create game objects
    std::cout << "Creating game objects..." << std::endl;
    
    // Background
    BGObject* bgObject = new BGObject("BGObject");
    GameObjectManager::getInstance()->addObject(bgObject);
    
    // TextureDisplay (will progressively load streaming textures and spawn icons)
    TextureDisplay* textureDisplay = new TextureDisplay();
    GameObjectManager::getInstance()->addObject(textureDisplay);
    
    // FPS Counter
    SimpleFPSCounter* fpsCounter = new SimpleFPSCounter();
    GameObjectManager::getInstance()->addObject(fpsCounter);
    
    std::cout << std::endl;
    std::cout << "Active objects: " << GameObjectManager::getInstance()->activeObjects() << std::endl;
    std::cout << std::endl;
    std::cout << "=== Instructions ===" << std::endl;
    std::cout << "1. Background will scroll continuously" << std::endl;
    std::cout << "2. Icons will appear in a grid (one per second)" << std::endl;
    std::cout << "3. Place 68×68 PNG images in Media/Streaming/ folder" << std::endl;
    std::cout << "4. Up to 50 icons will be loaded for this demo" << std::endl;
    std::cout << std::endl;
}

void BaseRunner::run() {
    sf::Clock clock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;
    
    while (window.isOpen()) {
        sf::Time elapsedTime = clock.restart();
        timeSinceLastUpdate += elapsedTime;
        
        while (timeSinceLastUpdate > TIME_PER_FRAME) {
            timeSinceLastUpdate -= TIME_PER_FRAME;
            processEvents();
            update(TIME_PER_FRAME);
        }
        
        render();
    }
    /*
    Better example of the while loop above:

    while window.isOpen() {                         // 1 main game tick = 1 display tick
        accumulatedTime += clock.restart()          // Collect time since last frame
        ticks = accumulatedTime // TIME_PER_FRAME   // Convert time into ticks
        accumulatedTime %= TIME_PER_FRAME           // Keep remaining time not counted as ticks
        
        for tick in range(ticks) {                  // Update game state for every tick
            processEvents()                         
            update(TIME_PER_FRAME)
        }                   
        render()                                    // Display current state 
    }                  
    */
}

void BaseRunner::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
        else
            GameObjectManager::getInstance()->processInput(event);
    }
}

void BaseRunner::update(sf::Time elapsedTime) {
    GameObjectManager::getInstance()->update(elapsedTime);
}

void BaseRunner::render() {
    window.clear(sf::Color::Black);
    GameObjectManager::getInstance()->draw(&window);
    window.display();
}
