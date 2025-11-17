#include <iostream>
#include <SFML/Graphics.hpp>

// New simplified architecture!
#include "core/Game.h"
#include "entities/Player.h"

using namespace std;


int main() {
    cout << "=== Simple 2D Game Engine - React-Style Lifecycle Hooks ===" << endl;
    cout << "Controls: WASD or Arrow Keys to move" << endl;
    cout << endl;

    // Create game instance
    auto game = Game(800, 600, "Simple 2D Game - Moving Circle");

    // Create and add player entity
    auto player = make_shared<Player>(sf::Vector2f(400, 300));
    game.addEntity(player);

    // Run the game loop
    game.run();

    cout << "Game ended!" << endl;
    return 0;
}
