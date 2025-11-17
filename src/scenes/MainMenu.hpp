#pragma once

#include "../core/Scene.hpp"
#include "../core/Game.hpp"
#include "../entities/MenuText.hpp"
#include "../entities/PressEnterText.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>

using namespace sf;
using namespace std;

// Forward declaration
class GamePlay;


// Main Menu
class MainMenu : public Scene {
public:
    MainMenu() : Scene("MainMenu") {}

    void onCreate() override {
        cout << "=== Main Menu Scene Loaded ===" << endl;

        // Create title
        auto title = make_shared<MenuText>(
            "Simple 2D Game Engine",
            Vector2f(400, 150),
            64
        );

        // Create subtitle
        auto subtitle = make_shared<MenuText>(
            "React-Style Lifecycle Hooks",
            Vector2f(400, 230),
            24
        );

        // Create pulsing instruction
        auto instruction = make_shared<PressEnterText>(
            Vector2f(400, 400)
        );

        // Add entities to scene
        this->addEntity(title);
        this->addEntity(subtitle);
        this->addEntity(instruction);
    }

    void onInput(Event& event) override {
        // Check for Enter key press event to start game
        if (event.type == Event::KeyPressed && event.key.code == Keyboard::Enter) {
            cout << "Starting game..." << endl;
            // Navigate to gameplay scene
            this->game->changeScene(this->createGamePlay());
        }
    }

    void onUpdate(Time dt) override {
        // Update all menu entities (for animations)
        this->updateEntities(dt);
    }

    void onDraw(RenderWindow& window) override {
        // Draw all menu entities
        this->drawEntities(window);
    }

    void onDestroy() override {
        cout << "=== Leaving Main Menu ===" << endl;
    }

private:
    shared_ptr<Scene> createGamePlay();  // Implemented after GamePlay definition
};
