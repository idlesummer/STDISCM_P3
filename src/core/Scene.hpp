#pragma once

#include "Entity.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <algorithm>

using namespace sf;
using namespace std;


// Forward declaration to avoid circular dependency
class Game;

// Base Scene class - like React Router pages/routes
class Scene {
public:
    Scene(const string& name = "Scene")
        : sceneName(name), game(nullptr) {}

    virtual ~Scene() = default;

    // Lifecycle hooks (React-style! - all use on* prefix for consistency)
    virtual void onCreate() {}                  // Like componentDidMount (scene enters)
    virtual void onInput() {}                   // Handle input (like event handlers)
    virtual void onUpdate(Time dt) {}           // Every frame while scene is active
    virtual void onDraw(RenderWindow& window) {} // Render the scene
    virtual void onDestroy() {}                 // Like componentWillUnmount (scene exits)

    // Entity management within this scene
    void addEntity(shared_ptr<Entity> entity) {
        entity->onCreate();
        entities.push_back(entity);
    }

    void removeEntity(shared_ptr<Entity> entity) {
        entity->onDestroy();
        auto it = find(entities.begin(), entities.end(), entity);
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

    string getName() const { return sceneName; }

protected:
    // Helper methods for derived scenes
    void inputEntities() {
        for (auto& entity : entities)
            if (entity->isActive())
                entity->onInput();
    }

    void updateEntities(Time dt) {
        for (auto& entity : entities)
            if (entity->isActive())
                entity->onUpdate(dt);
    }

    void drawEntities(RenderWindow& window) {
        for (auto& entity : entities)
            if (entity->isActive())
                entity->onDraw(window);
    }

    string sceneName;
    vector<shared_ptr<Entity>> entities;
    Game* game;  // Reference to game for scene transitions
};
