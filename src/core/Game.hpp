#pragma once

#include "Scene.hpp"
#include "Entity.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <algorithm>

using namespace sf;
using namespace std;


// Main game engine with scene management (like React Router!)
class Game {
public:
    Game(int width, int height, const string& title)
        : window(VideoMode(width, height), title), currentScene(nullptr) {
        window.setFramerateLimit(165);
    }

    // Scene management (like React Router navigation)
    void changeScene(shared_ptr<Scene> newScene) {
        if (currentScene) {
            currentScene->onDestroy();      // Unmount old scene
            currentScene->clearEntities();  // Cleanup entities
        }

        currentScene = newScene;

        if (currentScene) {
            currentScene->setGame(this);    // Give scene access to game
            currentScene->onCreate();       // Mount new scene
        }
    }

    shared_ptr<Scene> getCurrentScene() const {
        return currentScene;
    }

    // Legacy API: Add entity directly to game (no scene)
    // Kept for backward compatibility
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

    // Main game loop
    void run() {
        Clock clock;

        while (window.isOpen()) {
            Time dt = clock.restart();

            // 1. Process events and dispatch to scenes/entities
            Event event;
            while (window.pollEvent(event)) {
                // Handle window events
                if (event.type == Event::Closed)
                    window.close();

                // 2. Dispatch input events to current scene (if using scenes)
                if (currentScene) {
                    currentScene->onInput(event);
                }
                // Or input for entities directly (legacy mode)
                else {
                    for (auto& entity : entities)
                        if (entity->isActive())
                            entity->onInput(event);
                }
            }

            // 3. Update current scene (if using scenes)
            if (currentScene) {
                currentScene->onUpdate(dt);
            }
            // Or update entities directly (legacy mode)
            else {
                for (auto& entity : entities)
                    if (entity->isActive())
                        entity->onUpdate(dt);
            }

            // 4. Render phase - clear, draw, display
            window.clear(Color::Black);

            // Draw scene (if using scenes)
            if (currentScene) {
                currentScene->onDraw(window);
            }
            // Or draw entities directly (legacy mode)
            else {
                for (auto& entity : entities)
                    if (entity->isActive())
                        entity->onDraw(window);
            }

            window.display();
        }

        // Cleanup
        if (currentScene) {
            currentScene->onDestroy();
            currentScene->clearEntities();
        }
        for (auto& entity : entities)
            entity->onDestroy();
    }

    // Access to window
    RenderWindow& getWindow() { return window; }

private:
    RenderWindow window;
    shared_ptr<Scene> currentScene;
    vector<shared_ptr<Entity>> entities;  // Legacy: direct entities
};
