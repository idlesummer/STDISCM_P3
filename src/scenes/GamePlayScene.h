#pragma once

#include "../core/Scene.h"
#include "../core/Game.h"
#include "../entities/Player.h"
#include <SFML/Graphics.hpp>
#include <iostream>

// Forward declaration
class GameOverScene;


// Enemy entity that moves toward player
class Enemy : public Entity {
public:
    Enemy(sf::Vector2f initialPosition)
        : Entity("Enemy"), position(initialPosition), speed(3.0f) {}

    void onCreate() override {
        std::cout << "Enemy spawned!" << std::endl;

        sprite.setRadius(25.0f);
        sprite.setFillColor(sf::Color::Red);
        sprite.setOrigin(25.0f, 25.0f);
        sprite.setPosition(position);
    }

    void update(sf::Time dt) override {
        // Simple AI: move in a circular pattern
        elapsedTime += dt.asSeconds();
        position.x = 400 + 200 * std::cos(elapsedTime * speed * 0.1f);
        position.y = 300 + 200 * std::sin(elapsedTime * speed * 0.1f);

        sprite.setPosition(position);
    }

    void draw(sf::RenderWindow& window) override {
        window.draw(sprite);
    }

    sf::Vector2f getPosition() const { return position; }

private:
    sf::Vector2f position;
    float speed;
    float elapsedTime = 0.0f;
    sf::CircleShape sprite;
};


// Score display
class ScoreText : public Entity {
public:
    ScoreText() : Entity("ScoreText"), score(0) {}

    void onCreate() override {
        if (!font.loadFromFile("assets/sansation.ttf"))
            font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");

        text.setFont(font);
        text.setCharacterSize(24);
        text.setFillColor(sf::Color::White);
        text.setPosition(10, 10);
        updateText();
    }

    void update(sf::Time dt) override {
        score += dt.asSeconds() * 10;  // Score increases over time
        updateText();
    }

    void draw(sf::RenderWindow& window) override {
        window.draw(text);
    }

    int getScore() const { return static_cast<int>(score); }

private:
    void updateText() {
        text.setString("Score: " + std::to_string(static_cast<int>(score)));
    }

    sf::Font font;
    sf::Text text;
    float score;
};


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

    void onUpdate(sf::Time dt) override {
        // Update all entities
        updateEntities(dt);

        // Check for ESC to return to menu
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
            std::cout << "Returning to main menu..." << std::endl;
            game->changeScene(createMainMenuScene());
            return;
        }

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
