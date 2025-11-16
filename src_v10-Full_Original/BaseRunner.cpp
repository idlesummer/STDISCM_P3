#include "BaseRunner.h"
#include "GameObjectManager.h"
#include "BGObject.h"
#include "TextureManager.h"
#include "TextureDisplay.h"
#include "SimpleFPSCounter.h"
#include <iostream>

const sf::Time BaseRunner::TIME_PER_FRAME = sf::seconds(1.f / 60.f);

BaseRunner::BaseRunner() :
    window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Minimal v10: Full Original", sf::Style::Close) {
    std::cout << "=== Minimal v10: Full Original ===" << std::endl;
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
    std::cout << "1. Background scrolls continuously" << std::endl;
    std::cout << "2. Icons load with THREAD POOL (4 workers)" << std::endl;
    std::cout << "3. Multiple textures load SIMULTANEOUSLY!" << std::endl;
    std::cout << "4. Watch console for parallel loading!" << std::endl;
    std::cout << std::endl;
}

void BaseRunner::run() 
{
    sf::Clock clock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;
    
    while (window.isOpen())
    {
        sf::Time elapsedTime = clock.restart();
        timeSinceLastUpdate += elapsedTime;
        
        while (timeSinceLastUpdate > TIME_PER_FRAME)
        {
            timeSinceLastUpdate -= TIME_PER_FRAME;
            processEvents();
            update(TIME_PER_FRAME);
        }
        
        render();
    }
}

void BaseRunner::processEvents()
{
    sf::Event event;
    while (window.pollEvent(event)) 
    {
        if (event.type == sf::Event::Closed)
            window.close();
        else
            GameObjectManager::getInstance()->processInput(event);
    }
}

void BaseRunner::update(sf::Time elapsedTime) 
{
    GameObjectManager::getInstance()->update(elapsedTime);
}

void BaseRunner::render() 
{
    window.clear(sf::Color::Black);
    GameObjectManager::getInstance()->draw(&window);
    window.display();
}
