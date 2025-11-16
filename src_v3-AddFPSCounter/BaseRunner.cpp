#include "BaseRunner.h"
#include "GameObjectManager.h"
#include "SimpleCircle.h"
#include "SimpleFPSCounter.h"
#include <iostream>

const sf::Time BaseRunner::TIME_PER_FRAME = sf::seconds(1.f / 60.f);

BaseRunner::BaseRunner() :
    window(
        sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), 
        "Minimal v3: FPS Counter", 
        sf::Style::Close
    ) 
{
    std::cout << "Creating game objects..." << std::endl;
    
    // Create moving circles
    SimpleCircle* circle1 = new SimpleCircle("Circle1", 100, 300, 50);
    GameObjectManager::getInstance()->addObject(circle1);
    
    SimpleCircle* circle2 = new SimpleCircle("Circle2", 100, 500, 40);
    GameObjectManager::getInstance()->addObject(circle2);
    
    SimpleCircle* circle3 = new SimpleCircle("Circle3", 100, 700, 60);
    GameObjectManager::getInstance()->addObject(circle3);
    
    // Add FPS counter
    SimpleFPSCounter* fpsCounter = new SimpleFPSCounter();
    GameObjectManager::getInstance()->addObject(fpsCounter);
    
    std::cout << "Active objects: " << GameObjectManager::getInstance()->activeObjects() << std::endl;
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
