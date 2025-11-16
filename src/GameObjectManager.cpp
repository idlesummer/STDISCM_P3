#include <iostream>
#include "GameObjectManager.h"

using namespace std;
using namespace sf;


// ===== STATIC MEMBER INITIALIZATION ===== 
GameObjectManager* GameObjectManager::sharedInstance = nullptr;


// ===== CLASS IMPLEMENTATION ===== 
GameObjectManager::GameObjectManager() 
    : gameObjectMap(),
      gameObjectList() {

    // no-op
}

GameObjectManager* GameObjectManager::getInstance() {
    if (sharedInstance == nullptr)
        sharedInstance = new GameObjectManager();
    return sharedInstance;
}

AGameObject* GameObjectManager::findObjectByName(AGameObject::String name) {
    if (this->gameObjectMap[name] != nullptr)
        return this->gameObjectMap[name];
    
    cout << "Object not found: " << name << endl;
    return nullptr;
}

List GameObjectManager::getAllObjects() {
    return this->gameObjectList;
}

int GameObjectManager::activeObjects() {
    return this->gameObjectList.size();
}

void GameObjectManager::processInput(Event event) {
    for (int i = 0; i < this->gameObjectList.size(); i++)
        this->gameObjectList[i]->processInput(event);
}

void GameObjectManager::update(Time deltaTime) {
    for (int i = 0; i < this->gameObjectList.size(); i++)
        this->gameObjectList[i]->update(deltaTime);
}

void GameObjectManager::draw(RenderWindow* window) {
    for (int i = 0; i < this->gameObjectList.size(); i++)
        this->gameObjectList[i]->draw(window);
}

void GameObjectManager::addObject(AGameObject* gameObject) {
    this->gameObjectMap[gameObject->getName()] = gameObject;
    this->gameObjectList.push_back(gameObject);
    this->gameObjectMap[gameObject->getName()]->initialize();
}

void GameObjectManager::deleteObject(AGameObject* gameObject) {
    this->gameObjectMap.erase(gameObject->getName());
    
    int index = -1;
    for (int i = 0; i < this->gameObjectList.size(); i++) {
        if (this->gameObjectList[i] == gameObject) {
            index = i;
            break;
        }
    }
    
    if (index != -1)
        this->gameObjectList.erase(this->gameObjectList.begin() + index);
    
    delete gameObject;
}

void GameObjectManager::deleteObjectByName(AGameObject::String name) {
    auto object = this->findObjectByName(name);
    
    if (object == nullptr)
        return;
        
    this->deleteObject(object);
}
