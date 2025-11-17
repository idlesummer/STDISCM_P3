#pragma once

#include "Entity.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <algorithm>


// Forward declaration to avoid circular dependency
class Game;

// Base Scene class - like React Router pages/routes
class Scene {
public:
    Scene(const std::string& name = "Scene")
        : sceneName(name), game(nullptr) {}

    virtual ~Scene() = default;

    // Lifecycle hooks (React-style! - all use on* prefix for consistency)
    virtual void onCreate() {}                      // Like componentDidMount (scene enters)
    virtual void onInput() {}                       // Handle input (like event handlers)
    virtual void onUpdate(sf::Time dt) {}           // Every frame while scene is active
    virtual void onDraw(sf::RenderWindow& window) {} // Render the scene
    virtual void onDestroy() {}                     // Like componentWillUnmount (scene exits)

    // Entity management within this scene
    void addEntity(std::shared_ptr<Entity> entity) {
        entity->onCreate();
        entities.push_back(entity);
    }

    void removeEntity(std::shared_ptr<Entity> entity) {
        entity->onDestroy();
        auto it = std::find(entities.begin(), entities.end(), entity);
        if (it != entities.end())
            entities.erase(it);
    }

    // Clear all entities
    void clearEntities() {
        for (auto& entity : entities)
            entity->onDestroy();
        entities.clear();
    }

    // Access to game instance (for scene transitions)
    void setGame(Game* gameInstance) { game = gameInstance; }
    Game* getGame() const { return game; }

    std::string getName() const { return sceneName; }

protected:
    // Helper methods for derived scenes
    void inputEntities() {
        for (auto& entity : entities)
            if (entity->isActive())
                entity->onInput();
    }

    void updateEntities(sf::Time dt) {
        for (auto& entity : entities)
            if (entity->isActive())
                entity->onUpdate(dt);
    }

    void drawEntities(sf::RenderWindow& window) {
        for (auto& entity : entities)
            if (entity->isActive())
                entity->onDraw(window);
    }

    std::string sceneName;
    std::vector<std::shared_ptr<Entity>> entities;
    Game* game;  // Reference to game for scene transitions
};
