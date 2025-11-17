#pragma once

#include "../core/Scene.h"
#include "../core/Game.h"
#include <SFML/Graphics.hpp>
#include <iostream>

// Forward declaration
class GamePlayScene;


// Simple text entity for menu
class MenuText : public Entity {
public:
    MenuText(const std::string& content, sf::Vector2f position, int size = 48)
        : Entity("MenuText") {
        text.setString(content);
        text.setCharacterSize(size);
        text.setFillColor(sf::Color::White);
        text.setPosition(position);
    }

    void onCreate() override {
        // Try to load font (will use default if fails)
        if (!font.loadFromFile("assets/sansation.ttf"))
            font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");
        text.setFont(font);
    }

    void onDraw(sf::RenderWindow& window) override {
        window.draw(text);
    }

private:
    sf::Font font;
    sf::Text text;
};


// Pulsing "Press Enter" instruction
class PressEnterText : public Entity {
public:
    PressEnterText(sf::Vector2f position)
        : Entity("PressEnterText"), position(position), alpha(255), fadeDirection(-1) {}

    void onCreate() override {
        if (!font.loadFromFile("assets/sansation.ttf"))
            font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");

        text.setFont(font);
        text.setString("Press ENTER to Start");
        text.setCharacterSize(24);
        text.setPosition(position);

        // Center the text
        auto bounds = text.getLocalBounds();
        text.setOrigin(bounds.width / 2, bounds.height / 2);
    }

    void onUpdate(sf::Time dt) override {
        // Pulse effect
        alpha += fadeDirection * 200 * dt.asSeconds();

        if (alpha <= 100) {
            alpha = 100;
            fadeDirection = 1;
        } else if (alpha >= 255) {
            alpha = 255;
            fadeDirection = -1;
        }

        text.setFillColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(alpha)));
    }

    void onDraw(sf::RenderWindow& window) override {
        window.draw(text);
    }

private:
    sf::Font font;
    sf::Text text;
    sf::Vector2f position;
    float alpha;
    int fadeDirection;
};


// Main Menu Scene
class MainMenuScene : public Scene {
public:
    MainMenuScene() : Scene("MainMenu") {}

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

        // Center the title text
        // Note: This is a bit hacky since we can't access the text directly
        // In a real implementation, you'd want a better way to handle this
    }

    void onInput() override {
        // Check for Enter key to start game
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
            std::cout << "Starting game..." << std::endl;
            // Navigate to gameplay scene (defined below to avoid circular dependency)
            game->changeScene(createGamePlayScene());
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
    std::shared_ptr<Scene> createGamePlayScene();  // Implemented after GamePlayScene definition
};
