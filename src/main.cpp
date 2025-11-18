#include <iostream>
#include <SFML/Graphics.hpp>

// New scene-based architecture!
#include "core/Game.hpp"
#include "scenes/TetrisScene.hpp"

using namespace std;


int main() {
    cout << "============================================" << endl;
    cout << "  TETRIS - Minimal Implementation" << endl;
    cout << "  React-Style Lifecycle + Scene Management" << endl;
    cout << "============================================" << endl;
    cout << endl;

    // Create game instance
    auto game = Game(800, 700, "Tetris");

    // Start with Tetris scene
    game.changeScene(make_shared<TetrisScene>());

    // Run the game loop
    game.run();

    cout << endl;
    cout << "Game ended. Thanks for playing!" << endl;
    return 0;
}
