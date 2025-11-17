#pragma once

#include "../core/Scene.hpp"
#include "../core/Game.hpp"
#include "../entities/Player.hpp"
#include "../entities/Enemy.hpp"
#include "../entities/ScoreText.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>

using namespace sf;
using namespace std;

// Forward declaration
class GameOver;


// Gameplay
class GamePlay : public Scene {
public:
    GamePlay() : Scene("GamePlay") {}

    void onCreate() override {
        cout << "=== Game Started ===" << endl;

        // Create player
        this->player = make_shared<Player>(Vector2f(400, 300));
        this->addEntity(this->player);

        // Create enemies
        auto enemy1 = make_shared<Enemy>(Vector2f(200, 200));
        auto enemy2 = make_shared<Enemy>(Vector2f(600, 400));
        this->enemies.push_back(enemy1);
        this->enemies.push_back(enemy2);
        this->addEntity(enemy1);
        this->addEntity(enemy2);

        // Create score display
        this->scoreDisplay = make_shared<ScoreText>();
        this->addEntity(this->scoreDisplay);

        // Instructions
        if (this->instructionFont.loadFromFile("assets/fonts/sansation.ttf") ||
            this->instructionFont.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
            this->instructionText.setFont(this->instructionFont);
            this->instructionText.setString("WASD to move | ESC to return to menu | Survive!");
            this->instructionText.setCharacterSize(18);
            this->instructionText.setFillColor(Color(200, 200, 200));
            this->instructionText.setPosition(10, 570);
        }
    }

    void onInput(Event& event) override {
        // Dispatch input events to all entities
        this->inputEntities(event);

        // Check for ESC key press to return to menu
        if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape) {
            cout << "Returning to main menu..." << endl;
            this->game->changeScene(this->createMainMenu());
            return;
        }
    }

    void onUpdate(Time dt) override {
        // Update all entities
        this->updateEntities(dt);

        // Simple collision detection (game over if touching enemy)
        // Note: In a real game, you'd want proper collision detection
        // This is just for demonstration
        for (auto& enemy : this->enemies) {
            // Get positions (simplified - would need proper collision in real game)
            // For now, just check time - game over after 30 seconds
            if (this->elapsedTime > 30.0f) {
                cout << "Game Over!" << endl;
                this->game->changeScene(this->createGameOver(this->scoreDisplay->getScore()));
                return;
            }
        }

        this->elapsedTime += dt.asSeconds();
    }

    void onDraw(RenderWindow& window) override {
        // Draw all game entities
        this->drawEntities(window);

        // Draw instructions
        window.draw(this->instructionText);
    }

    void onDestroy() override {
        cout << "=== Leaving Gameplay ===" << endl;
    }

private:
    shared_ptr<Player> player;
    vector<shared_ptr<Enemy>> enemies;
    shared_ptr<ScoreText> scoreDisplay;
    float elapsedTime = 0.0f;

    Font instructionFont;
    Text instructionText;

    shared_ptr<Scene> createMainMenu();   // Forward declaration
    shared_ptr<Scene> createGameOver(int finalScore);  // Forward declaration
};
