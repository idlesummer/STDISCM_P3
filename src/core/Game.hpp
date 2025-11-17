#pragma once

#include "Scene.hpp"
#include "Entity.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <algorithm>


// Main game engine with scene management (like React Router!)
class Game {
public:
    Game(int width, int height, const std::string& title)
        : window(sf::VideoMode(width, height), title), currentScene(nullptr) {
        window.setFramerateLimit(165);
    }

    // Scene management (like React Router navigation)
    void changeScene(std::shared_ptr<Scene> newScene) {
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

    std::shared_ptr<Scene> getCurrentScene() const {
        return currentScene;
    }

    // Legacy API: Add entity directly to game (no scene)
    // Kept for backward compatibility
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

    // Main game loop
    void run() {
        sf::Clock clock;

        while (window.isOpen()) {
            sf::Time dt = clock.restart();

            // 1. Process events
            processEvents();

            // 2. Handle input for current scene (if using scenes)
            if (currentScene) {
                currentScene->onInput();
            }
            // Or input for entities directly (legacy mode)
            else {
                for (auto& entity : entities)
                    if (entity->isActive())
                        entity->onInput();
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
            window.clear(sf::Color::Black);

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
    sf::RenderWindow& getWindow() { return window; }

private:
    void processEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // You can add custom event handling here
            // or pass events to scenes/entities if needed
        }
    }

    sf::RenderWindow window;
    std::shared_ptr<Scene> currentScene;
    std::vector<std::shared_ptr<Entity>> entities;  // Legacy: direct entities
};
