#pragma once

#include "../core/Scene.hpp"
#include "../core/Game.hpp"
#include "../entities/GameOverText.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>

// Forward declaration
class MainMenuScene;


// Game Over Scene - receives score as parameter (like route params!)
class GameOverScene : public Scene {
public:
    GameOverScene(int finalScore)
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
            game->changeScene(createMainMenuScene());
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

    std::shared_ptr<Scene> createMainMenuScene();  // Forward declaration
};


// Now we can implement the forward-declared functions
#include "MainMenuScene.hpp"
#include "GamePlayScene.hpp"

// MainMenuScene navigation
std::shared_ptr<Scene> MainMenuScene::createGamePlayScene() {
    return std::make_shared<GamePlayScene>();
}

// GamePlayScene navigation
std::shared_ptr<Scene> GamePlayScene::createMainMenuScene() {
    return std::make_shared<MainMenuScene>();
}

std::shared_ptr<Scene> GamePlayScene::createGameOverScene(int finalScore) {
    return std::make_shared<GameOverScene>(finalScore);
}

// GameOverScene navigation
std::shared_ptr<Scene> GameOverScene::createMainMenuScene() {
    return std::make_shared<MainMenuScene>();
}
