#include "GameObjectManager.h"
#include <iostream>

GameObjectManager* GameObjectManager::sharedInstance = NULL;

GameObjectManager* GameObjectManager::getInstance() {
    if (sharedInstance == NULL) 
        sharedInstance = new GameObjectManager();
    return sharedInstance;
}

void GameObjectManager::addObject(AGameObject* gameObject) {
    gameObjectMap[gameObject->getName()] = gameObject;
    gameObjectList.push_back(gameObject);
    gameObject->initialize();
    
    std::cout << "Added object: " << gameObject->getName() << std::endl;
}

void GameObjectManager::processInput(sf::Event event)  {
    for (int i = 0; i < gameObjectList.size(); i++) 
        gameObjectList[i]->processInput(event);
}

void GameObjectManager::update(sf::Time deltaTime) {
    for (int i = 0; i < gameObjectList.size(); i++) 
        gameObjectList[i]->update(deltaTime);
}

void GameObjectManager::draw(sf::RenderWindow* window) {
    for (int i = 0; i < gameObjectList.size(); i++) 
        gameObjectList[i]->draw(window);
}

int GameObjectManager::activeObjects() {
    return gameObjectList.size();
}
