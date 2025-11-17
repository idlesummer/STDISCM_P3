#pragma once

#include "../core/Scene.hpp"
#include "../core/Game.hpp"
#include "../entities/GameOverText.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>

// Forward declaration
class MainMenu;


// Game Over - receives score as parameter (like route params!)
class GameOver : public Scene {
public:
    GameOver(int finalScore)
        : Scene("GameOver"), finalScore(finalScore) {}

    void onCreate() override {
        std::cout << "=== Game Over! Final Score: " << finalScore << " ===" << std::endl;

        // Game Over title
        auto title = std::make_shared<GameOverText>(
            "GAME OVER",
            sf::Vector2f(400, 150),
            72,
            sf::Color::Red
        );
        addEntity(title);

        // Score display
        auto scoreText = std::make_shared<GameOverText>(
            "Final Score: " + std::to_string(finalScore),
            sf::Vector2f(400, 280),
            36,
            sf::Color::Yellow
        );
        addEntity(scoreText);

        // Performance message
        std::string message;
        if (finalScore > 250)
            message = "Amazing!";
        else if (finalScore > 200)
            message = "Great Job!";
        else if (finalScore > 150)
            message = "Good!";
        else
            message = "Keep Practicing!";

        auto messageText = std::make_shared<GameOverText>(
            message,
            sf::Vector2f(400, 350),
            28,
            sf::Color::White
        );
        addEntity(messageText);

        // Instructions
        auto instruction = std::make_shared<GameOverText>(
            "Press ENTER to return to Main Menu",
            sf::Vector2f(400, 450),
            24,
            sf::Color(200, 200, 200)
        );
        addEntity(instruction);
    }

    void onInput() override {
        // Check for Enter to return to menu
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
            std::cout << "Returning to main menu..." << std::endl;
            game->changeScene(createMainMenu());
        }
    }

    void onUpdate(sf::Time dt) override {
        updateEntities(dt);
    }

    void onDraw(sf::RenderWindow& window) override {
        drawEntities(window);
    }

    void onDestroy() override {
        std::cout << "=== Leaving Game Over Screen ===" << std::endl;
    }

private:
    int finalScore;

    std::shared_ptr<Scene> createMainMenu();  // Forward declaration
};


// Now we can implement the forward-declared functions
#include "MainMenu.hpp"
#include "GamePlay.hpp"

// MainMenu navigation
std::shared_ptr<Scene> MainMenu::createGamePlay() {
    return std::make_shared<GamePlay>();
}

// GamePlay navigation
std::shared_ptr<Scene> GamePlay::createMainMenu() {
    return std::make_shared<MainMenu>();
}

std::shared_ptr<Scene> GamePlay::createGameOver(int finalScore) {
    return std::make_shared<GameOver>(finalScore);
}

// GameOver navigation
std::shared_ptr<Scene> GameOver::createMainMenu() {
    return std::make_shared<MainMenu>();
}
