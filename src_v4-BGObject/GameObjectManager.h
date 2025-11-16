#pragma once
#include <unordered_map>
#include <vector>
#include <string>
#include "AGameObject.h"
#include <SFML/Graphics.hpp>

typedef std::unordered_map<std::string, AGameObject*> HashTable;
typedef std::vector<AGameObject*> List;

class GameObjectManager
{
public:
    static GameObjectManager* getInstance();
    
    void addObject(AGameObject* gameObject);
    void processInput(sf::Event event);
    void update(sf::Time deltaTime);
    void draw(sf::RenderWindow* window);
    
    int activeObjects();

private:
    GameObjectManager() {};
    GameObjectManager(GameObjectManager const&) {};
    GameObjectManager& operator=(GameObjectManager const&) {};
    static GameObjectManager* sharedInstance;

    HashTable gameObjectMap;
    List gameObjectList;
};
