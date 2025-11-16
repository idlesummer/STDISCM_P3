#include "BaseRunner.h"
#include "GameObjectManager.h"
#include "BGObject.h"
#include "TextureManager.h"
#include "SimpleFPSCounter.h"
#include <iostream>

const sf::Time BaseRunner::TIME_PER_FRAME = sf::seconds(1.f / 60.f);

BaseRunner::BaseRunner() :
    window(
        sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), 
        "Minimal v4: Scrolling Background + TextureManager", 
        sf::Style::Close
    ) 
{
    std::cout << "=== Minimal v4: Scrolling Background ===" << std::endl;
    std::cout << std::endl;
    
    // STEP 1: Load textures using TextureManager
    std::cout << "Loading textures..." << std::endl;
    TextureManager::getInstance()->loadTexture("Desert", "Media/Textures/desert.png");
    std::cout << std::endl;
    
    // STEP 2: Create game objects
    std::cout << "Creating game objects..." << std::endl;
    
    BGObject* bgObject = new BGObject("BGObject");
    GameObjectManager::getInstance()->addObject(bgObject);
    
    SimpleFPSCounter* fpsCounter = new SimpleFPSCounter();
    GameObjectManager::getInstance()->addObject(fpsCounter);
    
    std::cout << std::endl;
    std::cout << "Active objects: " << GameObjectManager::getInstance()->activeObjects() << std::endl;
    std::cout << "Background should be scrolling slowly downward!" << std::endl;
    std::cout << "If you see a black screen, make sure 'desert.png' exists!" << std::endl;
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
