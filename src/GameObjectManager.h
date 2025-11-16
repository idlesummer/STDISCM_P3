#pragma once
#include <unordered_map>
#include <vector>
#include <string>
#include "AGameObject.h"
#include <SFML/Graphics.hpp>

using HashTable = std::unordered_map<std::string, AGameObject*>;
using List = std::vector<AGameObject*>;

class GameObjectManager {
public:
    static GameObjectManager* getInstance();
    
    AGameObject* findObjectByName(AGameObject::String name);
    List getAllObjects();
    int activeObjects();
    
    void processInput(sf::Event event);
    void update(sf::Time deltaTime);
    void draw(sf::RenderWindow* window);
    void addObject(AGameObject* gameObject);
    void deleteObject(AGameObject* gameObject);
    void deleteObjectByName(AGameObject::String name);

private:
    GameObjectManager();
    GameObjectManager(GameObjectManager const&) {}
    GameObjectManager& operator=(GameObjectManager const&) { return *this; }
    
    static GameObjectManager* sharedInstance;
    
    HashTable gameObjectMap;
    List gameObjectList;
};
