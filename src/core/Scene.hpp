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
protected:
    string sceneName;
    vector<shared_ptr<Entity>> entities;
    Game* game;  // Reference to game for scene transitions

public:
    Scene(const string& name = "Scene")
        : sceneName(name),
          game(nullptr) {}

    virtual ~Scene() = default;

    // Lifecycle hooks (React-style! - all use on* prefix for consistency)
    virtual void onCreate() {}                  // Like componentDidMount (scene enters)
    virtual void onInput(Event& event) {}       // Handle input events (like onClick, onKeyPress)
    virtual void onUpdate(Time dt) {}           // Every frame while scene is active
    virtual void onDraw(RenderWindow& window) {} // Render the scene
    virtual void onDestroy() {}                 // Like componentWillUnmount (scene exits)

    // Entity management within this scene
    void addEntity(shared_ptr<Entity> entity) {
        entity->onCreate();
        this->entities.push_back(entity);
    }

    void removeEntity(shared_ptr<Entity> entity) {
        entity->onDestroy();
        auto it = find(this->entities.begin(), this->entities.end(), entity);
        if (it != this->entities.end())
            this->entities.erase(it);
    }

    // Clear all entities
    void clearEntities() {
        for (auto& entity : this->entities)
            entity->onDestroy();
        this->entities.clear();
    }

    // Access to game instance (for scene transitions)
    void setGame(Game* gameInstance) { this->game = gameInstance; }
    auto getName() const { return this->sceneName; }

protected:
    // Helper methods for derived scenes
    void inputEntities(Event& event) {
        for (auto& entity : this->entities)
            if (entity->isActive())
                entity->onInput(event);
    }

    void updateEntities(Time dt) {
        for (auto& entity : this->entities)
            if (entity->isActive())
                entity->onUpdate(dt);
    }

    void drawEntities(RenderWindow& window) {
        for (auto& entity : this->entities)
            if (entity->isActive() && entity->isVisible())
                entity->onDraw(window);
    }
};
