#pragma once

#include "Entity.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <algorithm>


// Main game engine - simplified, no virtual DOM!
class Game {
public:
    Game(int width, int height, const std::string& title)
        : window(sf::VideoMode(width, height), title) {
        window.setFramerateLimit(165);
    }

    // Add entity to the game world
    void addEntity(std::shared_ptr<Entity> entity) {
        entity->onCreate();  // Call lifecycle hook
        entities.push_back(entity);
    }

    // Remove entity by reference
    void removeEntity(std::shared_ptr<Entity> entity) {
        entity->onDestroy();  // Call lifecycle hook
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

            // 2. Update all active entities
            for (auto& entity : entities)
                if (entity->isActive())
                    entity->update(dt);

            // 3. Render phase - clear, draw, display
            window.clear(sf::Color::Black);

            for (auto& entity : entities)
                if (entity->isActive())
                    entity->draw(window);

            window.display();
        }

        // Cleanup - call onDestroy for all entities
        for (auto& entity : entities)
            entity->onDestroy();
    }

    // Access to window for custom event handling
    sf::RenderWindow& getWindow() { return window; }

private:
    void processEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // You can add custom event handling here
            // or pass events to entities if needed
        }
    }

    sf::RenderWindow window;
    std::vector<std::shared_ptr<Entity>> entities;
};
