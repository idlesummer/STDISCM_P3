#include <iostream>
#include <SFML/Graphics.hpp>

// New scene-based architecture!
#include "core/Game.hpp"
#include "scenes/MainMenuScene.hpp"
#include "scenes/GamePlayScene.hpp"
#include "scenes/GameOverScene.hpp"

using namespace std;


int main() {
    cout << "============================================" << endl;
    cout << "  Simple 2D Game Engine" << endl;
    cout << "  React-Style Lifecycle + Scene Management" << endl;
    cout << "============================================" << endl;
    cout << endl;

    // Create game instance
    auto game = Game(800, 600, "Simple 2D Game - Scene System Demo");

    // Start with main menu scene (like initial route in React Router!)
    game.changeScene(make_shared<MainMenuScene>());

    // Run the game loop
    game.run();

    cout << endl;
    cout << "Game ended. Thanks for playing!" << endl;
    return 0;
}
