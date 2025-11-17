#pragma once

#include "../core/Scene.hpp"
#include "../core/Game.hpp"
#include "../entities/Player.hpp"
#include "../entities/Enemy.hpp"
#include "../entities/ScoreText.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>

// Forward declaration
class GameOverScene;


// Gameplay Scene
class GamePlayScene : public Scene {
public:
    GamePlayScene() : Scene("GamePlay") {}

    void onCreate() override {
        std::cout << "=== Game Started ===" << std::endl;

        // Create player
        player = std::make_shared<Player>(sf::Vector2f(400, 300));
        addEntity(player);

        // Create enemies
        auto enemy1 = std::make_shared<Enemy>(sf::Vector2f(200, 200));
        auto enemy2 = std::make_shared<Enemy>(sf::Vector2f(600, 400));
        enemies.push_back(enemy1);
        enemies.push_back(enemy2);
        addEntity(enemy1);
        addEntity(enemy2);

        // Create score display
        scoreDisplay = std::make_shared<ScoreText>();
        addEntity(scoreDisplay);

        // Instructions
        if (instructionFont.loadFromFile("assets/sansation.ttf") ||
            instructionFont.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
            instructionText.setFont(instructionFont);
            instructionText.setString("WASD to move | ESC to return to menu | Survive!");
            instructionText.setCharacterSize(18);
            instructionText.setFillColor(sf::Color(200, 200, 200));
            instructionText.setPosition(10, 570);
        }
    }

    void onInput() override {
        // Handle input for all entities (player movement, etc.)
        inputEntities();

        // Check for ESC to return to menu
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
            std::cout << "Returning to main menu..." << std::endl;
            game->changeScene(createMainMenuScene());
            return;
        }
    }

    void onUpdate(sf::Time dt) override {
        // Update all entities
        updateEntities(dt);

        // Simple collision detection (game over if touching enemy)
        // Note: In a real game, you'd want proper collision detection
        // This is just for demonstration
        for (auto& enemy : enemies) {
            // Get positions (simplified - would need proper collision in real game)
            // For now, just check time - game over after 30 seconds
            if (elapsedTime > 30.0f) {
                std::cout << "Game Over!" << std::endl;
                game->changeScene(createGameOverScene(scoreDisplay->getScore()));
                return;
            }
        }

        elapsedTime += dt.asSeconds();
    }

    void onDraw(sf::RenderWindow& window) override {
        // Draw all game entities
        drawEntities(window);

        // Draw instructions
        window.draw(instructionText);
    }

    void onDestroy() override {
        std::cout << "=== Leaving Gameplay ===" << std::endl;
    }

private:
    std::shared_ptr<Player> player;
    std::vector<std::shared_ptr<Enemy>> enemies;
    std::shared_ptr<ScoreText> scoreDisplay;
    float elapsedTime = 0.0f;

    sf::Font instructionFont;
    sf::Text instructionText;

    std::shared_ptr<Scene> createMainMenuScene();   // Forward declaration
    std::shared_ptr<Scene> createGameOverScene(int finalScore);  // Forward declaration
};
