#include <iostream>
#include <SFML/Graphics.hpp>

// Core engine
#include "core/ReactSFMLEngine.h"

// Components
#include "components/GameState.h"
#include "components/GameComponent.h"

using namespace ReactSFML;

// Event handler - converts SFML events to actions
void handleEvents(sf::Event& event, Store<GameState>& store) {
    const auto& state = store.getState();

    // Keyboard input for player movement
    if (event.type == sf::Event::KeyPressed) {
        sf::Vector2f delta(0, 0);
        float speed = 5.0f;

        switch (event.key.code) {
            case sf::Keyboard::Left:
            case sf::Keyboard::A:
                delta.x = -speed;
                break;
            case sf::Keyboard::Right:
            case sf::Keyboard::D:
                delta.x = speed;
                break;
            case sf::Keyboard::Up:
            case sf::Keyboard::W:
                delta.y = -speed;
                break;
            case sf::Keyboard::Down:
            case sf::Keyboard::S:
                delta.y = speed;
                break;
            case sf::Keyboard::Space:
                // Spawn enemy on spacebar
                if (!state.gameOver) {
                    static int enemyIdCounter = 0;
                    EnemyData enemy;
                    enemy.id = enemyIdCounter++;
                    enemy.position = sf::Vector2f(rand() % 700 + 50, 50);
                    enemy.health = 50;
                    enemy.speed = 2.0f;

                    store.dispatch(createSpawnEnemyAction(enemy));
                }
                break;
            case sf::Keyboard::H:
                // Test damage
                store.dispatch(createDamagePlayerAction(10));
                break;
            default:
                break;
        }

        if (delta.x != 0 || delta.y != 0) {
            store.dispatch(createMovePlayerAction(delta));
        }
    }
}

// Main entry point
int main() {
    std::cout << "=== React-Inspired SFML Game Engine ===" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  WASD / Arrow Keys - Move player" << std::endl;
    std::cout << "  Space - Spawn enemy" << std::endl;
    std::cout << "  H - Take damage (test)" << std::endl;
    std::cout << std::endl;

    // Create engine with initial state and reducer
    ReactSFMLEngine<GameState> engine(
        800, 600,
        "React SFML - Demo Game",
        GameState(),
        gameReducer
    );

    // Optional: Add logger middleware
    engine.getStore().addMiddleware(createLoggerMiddleware());

    // Set event handler
    engine.setEventHandler(handleEvents);

    // Create root game component
    auto gameComponent = std::make_shared<GameComponent>(&engine.getStore());
    engine.setRoot(gameComponent);

    // Run the game loop
    engine.run();

    std::cout << "Game ended!" << std::endl;

    return 0;
}
