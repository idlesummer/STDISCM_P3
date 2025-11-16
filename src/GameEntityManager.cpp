#include <iostream>
#include "GameEntityManager.h"

using namespace std;
using namespace sf;


// ===== STATIC MEMBER INITIALIZATION =====
GameEntityManager* GameEntityManager::sharedInstance = nullptr;


// ===== CLASS IMPLEMENTATION =====
GameEntityManager::GameEntityManager()
    : entityMap(),
      entityList() {

    // no-op
}

GameEntityManager* GameEntityManager::getInstance() {
    if (sharedInstance == nullptr)
        sharedInstance = new GameEntityManager();
    return sharedInstance;
}

GameEntity* GameEntityManager::findEntityByName(GameEntity::String name) {
    if (this->entityMap[name] != nullptr)
        return this->entityMap[name];

    cout << "Entity not found: " << name << endl;
    return nullptr;
}

EntityList GameEntityManager::getAllEntities() {
    return this->entityList;
}

int GameEntityManager::activeEntities() {
    return this->entityList.size();
}

void GameEntityManager::processInput(Event event) {
    for (int i = 0; i < this->entityList.size(); i++)
        this->entityList[i]->processInput(event);
}

void GameEntityManager::update(Time deltaTime) {
    for (int i = 0; i < this->entityList.size(); i++)
        this->entityList[i]->update(deltaTime);
}

void GameEntityManager::draw(RenderWindow* window) {
    for (int i = 0; i < this->entityList.size(); i++)
        this->entityList[i]->draw(window);
}

void GameEntityManager::addEntity(GameEntity* entity) {
    this->entityMap[entity->getName()] = entity;
    this->entityList.push_back(entity);
    this->entityMap[entity->getName()]->initialize();
}

void GameEntityManager::deleteEntity(GameEntity* entity) {
    this->entityMap.erase(entity->getName());

    int index = -1;
    for (int i = 0; i < this->entityList.size(); i++) {
        if (this->entityList[i] == entity) {
            index = i;
            break;
        }
    }

    if (index != -1)
        this->entityList.erase(this->entityList.begin() + index);

    delete entity;
}

void GameEntityManager::deleteEntityByName(GameEntity::String name) {
    auto entity = this->findEntityByName(name);

    if (entity == nullptr)
        return;

    this->deleteEntity(entity);
}
