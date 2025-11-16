#include <iostream>
#include "EntityManager.h"

using namespace std;
using namespace sf;


// ===== STATIC MEMBER INITIALIZATION =====
EntityManager* EntityManager::sharedInstance = nullptr;


// ===== CLASS IMPLEMENTATION =====
EntityManager::EntityManager()
    : entityMap(),
      entityList() {

    // no-op
}

EntityManager* EntityManager::getInstance() {
    if (sharedInstance == nullptr)
        sharedInstance = new EntityManager();
    return sharedInstance;
}

GameEntity* EntityManager::findEntityByName(GameEntity::String name) {
    if (this->entityMap[name] != nullptr)
        return this->entityMap[name];

    cout << "Entity not found: " << name << endl;
    return nullptr;
}

EntityList EntityManager::getAllEntities() {
    return this->entityList;
}

int EntityManager::activeEntities() {
    return this->entityList.size();
}

void EntityManager::processInput(Event event) {
    for (int i = 0; i < this->entityList.size(); i++)
        this->entityList[i]->processInput(event);
}

void EntityManager::update(Time deltaTime) {
    for (int i = 0; i < this->entityList.size(); i++)
        this->entityList[i]->update(deltaTime);
}

void EntityManager::draw(RenderWindow* window) {
    for (int i = 0; i < this->entityList.size(); i++)
        this->entityList[i]->draw(window);
}

void EntityManager::addEntity(GameEntity* entity) {
    this->entityMap[entity->getName()] = entity;
    this->entityList.push_back(entity);
    this->entityMap[entity->getName()]->initialize();
}

void EntityManager::deleteEntity(GameEntity* entity) {
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

void EntityManager::deleteEntityByName(GameEntity::String name) {
    auto entity = this->findEntityByName(name);

    if (entity == nullptr)
        return;

    this->deleteEntity(entity);
}
