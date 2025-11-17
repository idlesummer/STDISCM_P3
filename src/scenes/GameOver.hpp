#pragma once

#include "../core/Scene.hpp"
#include "../core/Game.hpp"
#include "../entities/GameOverText.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>

using namespace sf;
using namespace std;

// Forward declaration
class MainMenu;


// Game Over - receives score as parameter (like route params!)
class GameOver : public Scene {
public:
    GameOver(int finalScore)
        : Scene("GameOver"), finalScore(finalScore) {}

    void onCreate() override {
        cout << "=== Game Over! Final Score: " << this->finalScore << " ===" << endl;

        // Game Over title
        auto title = make_shared<GameOverText>(
            "GAME OVER",
            Vector2f(400, 150),
            72,
            Color::Red
        );
        this->addEntity(title);

        // Score display
        auto scoreText = make_shared<GameOverText>(
            "Final Score: " + to_string(this->finalScore),
            Vector2f(400, 280),
            36,
            Color::Yellow
        );
        this->addEntity(scoreText);

        // Performance message
        string message;
        if (this->finalScore > 250)
            message = "Amazing!";
        else if (this->finalScore > 200)
            message = "Great Job!";
        else if (this->finalScore > 150)
            message = "Good!";
        else
            message = "Keep Practicing!";

        auto messageText = make_shared<GameOverText>(
            message,
            Vector2f(400, 350),
            28,
            Color::White
        );
        this->addEntity(messageText);

        // Instructions
        auto instruction = make_shared<GameOverText>(
            "Press ENTER to return to Main Menu",
            Vector2f(400, 450),
            24,
            Color(200, 200, 200)
        );
        this->addEntity(instruction);
    }

    void onInput(Event& event) override {
        // Check for Enter key press event to return to menu
        if (event.type == Event::KeyPressed && event.key.code == Keyboard::Enter) {
            cout << "Returning to main menu..." << endl;
            this->game->changeScene(this->createMainMenu());
        }
    }

    void onUpdate(Time dt) override {
        this->updateEntities(dt);
    }

    void onDraw(RenderWindow& window) override {
        this->drawEntities(window);
    }

    void onDestroy() override {
        cout << "=== Leaving Game Over Screen ===" << endl;
    }

private:
    int finalScore;

    shared_ptr<Scene> createMainMenu();  // Forward declaration
};


// Now we can implement the forward-declared functions
#include "MainMenu.hpp"
#include "GamePlay.hpp"

// MainMenu navigation
shared_ptr<Scene> MainMenu::createGamePlay() {
    return make_shared<GamePlay>();
}

// GamePlay navigation
shared_ptr<Scene> GamePlay::createMainMenu() {
    return make_shared<MainMenu>();
}

shared_ptr<Scene> GamePlay::createGameOver(int finalScore) {
    return make_shared<GameOver>(finalScore);
}

// GameOver navigation
shared_ptr<Scene> GameOver::createMainMenu() {
    return make_shared<MainMenu>();
}
