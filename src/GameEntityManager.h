#pragma once
#include <unordered_map>
#include <vector>
#include <string>
#include "GameEntity.h"
#include <SFML/Graphics.hpp>

using EntityHashTable = std::unordered_map<std::string, GameEntity*>;
using EntityList = std::vector<GameEntity*>;

class GameEntityManager {
public:
    static GameEntityManager* getInstance();

    GameEntity* findEntityByName(GameEntity::String name);
    EntityList getAllEntities();
    int activeEntities();

    void processInput(sf::Event event);
    void update(sf::Time deltaTime);
    void draw(sf::RenderWindow* window);
    void addEntity(GameEntity* entity);
    void deleteEntity(GameEntity* entity);
    void deleteEntityByName(GameEntity::String name);

private:
    GameEntityManager();
    GameEntityManager(GameEntityManager const&) = delete;
    GameEntityManager& operator=(GameEntityManager const&) = delete;

    static GameEntityManager* sharedInstance;

    EntityHashTable entityMap;
    EntityList entityList;
};
