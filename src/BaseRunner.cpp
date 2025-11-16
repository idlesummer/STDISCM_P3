#include <iostream>
#include "BaseRunner.h"
#include "GameObjectManager.h"
#include "SystemManager.h"
#include "BGObject.h"
#include "TextureManager.h"
#include "IconSpawnerSystem.h"
#include "FPSCounter.h"

using namespace std;
using namespace sf;


// ===== STATIC MEMBER INITIALIZATION ===== 
const int BaseRunner::WINDOW_WIDTH = 1920;
const int BaseRunner::WINDOW_HEIGHT = 1080;
const Time BaseRunner::TIME_PER_FRAME = seconds(1.f / 60.f);    // Alternative name: TICK


// ===== CLASS IMPLEMENTATION ===== 
BaseRunner::BaseRunner() 
    : window(
        VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), 
        "HO: Entity Component", 
        Style::Close
    ) {

    cout << "=== Minimal v10: Full Original (Styled Complete) ===" << endl;
    cout << endl;
    cout << "=== Instructions ===" << endl;
    cout << "1. Background scrolls continuously" << endl;
    cout << "2. Icons load with THREAD POOL (4 workers)" << endl;
    cout << "3. Multiple textures load SIMULTANEOUSLY!" << endl;
    cout << "4. All member variables initialized in constructor!" << endl;
    cout << endl;

    auto* textureManager = TextureManager::getInstance();
    auto* gameObjectManager = GameObjectManager::getInstance();
    auto* systemManager = SystemManager::getInstance();

    // Load base textures
    textureManager->loadTexture("Desert", "Media/Textures/Desert.png");

    // Create visual game objects (things that render)
    auto bgObject = new BGObject("BGObject");
    gameObjectManager->addObject(bgObject);

    auto fpsCounter = new FPSCounter();
    gameObjectManager->addObject(fpsCounter);

    // Create game systems (logic controllers that don't render)
    auto iconSpawner = new IconSpawnerSystem();
    systemManager->addSystem(iconSpawner);

    // Initialize all systems
    systemManager->initializeAll();
}

void BaseRunner::run() {
    auto clock = Clock();
    auto lag = Time::Zero;

    while (this->window.isOpen()) {              
        for (lag += clock.restart(); lag >= TIME_PER_FRAME; lag -= TIME_PER_FRAME) { // 1 iter == 1 tick  
            this->processEvents();
            this->update(TIME_PER_FRAME);
        }
        this->render();
    }
}

void BaseRunner::processEvents() {
    auto event = Event();
    
    if (!this->window.pollEvent(event))
        return;
    
    switch (event.type) {
        default:
            GameObjectManager::getInstance()->processInput(event);
            return;

        case Event::Closed:
            this->window.close();
            return; 
    }    
}

void BaseRunner::update(Time elapsedTime) {
    // Update game systems (controllers/logic)
    SystemManager::getInstance()->update(elapsedTime);

    // Update game objects (visual entities)
    GameObjectManager::getInstance()->update(elapsedTime);
}

void BaseRunner::render() {
    this->window.clear();
    GameObjectManager::getInstance()->draw(&this->window);
    this->window.display();
}
