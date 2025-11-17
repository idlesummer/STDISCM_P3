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
        player = make_shared<Player>(Vector2f(400, 300));
        addEntity(player);

        // Create enemies
        auto enemy1 = make_shared<Enemy>(Vector2f(200, 200));
        auto enemy2 = make_shared<Enemy>(Vector2f(600, 400));
        enemies.push_back(enemy1);
        enemies.push_back(enemy2);
        addEntity(enemy1);
        addEntity(enemy2);

        // Create score display
        scoreDisplay = make_shared<ScoreText>();
        addEntity(scoreDisplay);

        // Instructions
        if (instructionFont.loadFromFile("assets/sansation.ttf") ||
            instructionFont.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
            instructionText.setFont(instructionFont);
            instructionText.setString("WASD to move | ESC to return to menu | Survive!");
            instructionText.setCharacterSize(18);
            instructionText.setFillColor(Color(200, 200, 200));
            instructionText.setPosition(10, 570);
        }
    }

    void onInput() override {
        // Handle input for all entities (player movement, etc.)
        inputEntities();

        // Check for ESC to return to menu
        if (Keyboard::isKeyPressed(Keyboard::Escape)) {
            cout << "Returning to main menu..." << endl;
            game->changeScene(createMainMenu());
            return;
        }
    }

    void onUpdate(Time dt) override {
        // Update all entities
        updateEntities(dt);

        // Simple collision detection (game over if touching enemy)
        // Note: In a real game, you'd want proper collision detection
        // This is just for demonstration
        for (auto& enemy : enemies) {
            // Get positions (simplified - would need proper collision in real game)
            // For now, just check time - game over after 30 seconds
            if (elapsedTime > 30.0f) {
                cout << "Game Over!" << endl;
                game->changeScene(createGameOver(scoreDisplay->getScore()));
                return;
            }
        }

        elapsedTime += dt.asSeconds();
    }

    void onDraw(RenderWindow& window) override {
        // Draw all game entities
        drawEntities(window);

        // Draw instructions
        window.draw(instructionText);
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
