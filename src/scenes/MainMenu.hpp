#pragma once

#include "../core/Scene.hpp"
#include "../core/Game.hpp"
#include "../entities/MenuText.hpp"
#include "../entities/PressEnterText.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>

// Forward declaration
class GamePlay;


// Main Menu
class MainMenu : public Scene {
public:
    MainMenu() : Scene("MainMenu") {}

    void onCreate() override {
        std::cout << "=== Main Menu Scene Loaded ===" << std::endl;

        // Create title
        auto title = std::make_shared<MenuText>(
            "Simple 2D Game Engine",
            sf::Vector2f(400, 150),
            64
        );

        // Create subtitle
        auto subtitle = std::make_shared<MenuText>(
            "React-Style Lifecycle Hooks",
            sf::Vector2f(400, 230),
            24
        );

        // Create pulsing instruction
        auto instruction = std::make_shared<PressEnterText>(
            sf::Vector2f(400, 400)
        );

        // Add entities to scene
        addEntity(title);
        addEntity(subtitle);
        addEntity(instruction);
    }

    void onInput() override {
        // Check for Enter key to start game
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
            std::cout << "Starting game..." << std::endl;
            // Navigate to gameplay scene
            game->changeScene(createGamePlay());
        }
    }

    void onUpdate(sf::Time dt) override {
        // Update all menu entities (for animations)
        updateEntities(dt);
    }

    void onDraw(sf::RenderWindow& window) override {
        // Draw all menu entities
        drawEntities(window);
    }

    void onDestroy() override {
        std::cout << "=== Leaving Main Menu ===" << std::endl;
    }

private:
    std::shared_ptr<Scene> createGamePlay();  // Implemented after GamePlay definition
};
