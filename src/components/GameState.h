#pragma once

#include "../core/Store.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <algorithm>

namespace Reactive {

// Game entity data
struct EnemyData {
    int id;
    sf::Vector2f position;
    int health;
    float speed;
};

// Main game state
struct GameState : public State {
    sf::Vector2f playerPosition;
    int playerHealth;
    int score;
    std::vector<EnemyData> enemies;
    bool gameOver;
    float time;

    GameState()
        : playerPosition(400, 500),
          playerHealth(100),
          score(0),
          gameOver(false),
          time(0.0f) {}

    State* clone() const override {
        return new GameState(*this);
    }
};

// Action creators
inline Action createMovePlayerAction(sf::Vector2f delta) {
    return Action{"MOVE_PLAYER", delta};
}

inline Action createSpawnEnemyAction(EnemyData enemy) {
    return Action{"SPAWN_ENEMY", enemy};
}

inline Action createDamagePlayerAction(int damage) {
    return Action{"DAMAGE_PLAYER", damage};
}

inline Action createKillEnemyAction(int enemyId) {
    return Action{"KILL_ENEMY", enemyId};
}

inline Action createUpdateTimeAction(float deltaTime) {
    return Action{"UPDATE_TIME", deltaTime};
}

// Main reducer
inline GameState gameReducer(const GameState& state, const Action& action) {
    GameState newState = state;

    if (action.type == "MOVE_PLAYER") {
        auto delta = action.getPayload<sf::Vector2f>();
        newState.playerPosition = state.playerPosition + delta;

        // Clamp to screen bounds
        newState.playerPosition.x = std::max(0.0f, std::min(800.0f, newState.playerPosition.x));
        newState.playerPosition.y = std::max(0.0f, std::min(600.0f, newState.playerPosition.y));
    }
    else if (action.type == "SPAWN_ENEMY") {
        auto enemy = action.getPayload<EnemyData>();
        newState.enemies.push_back(enemy);
    }
    else if (action.type == "DAMAGE_PLAYER") {
        auto damage = action.getPayload<int>();
        newState.playerHealth = std::max(0, state.playerHealth - damage);

        if (newState.playerHealth == 0) {
            newState.gameOver = true;
        }
    }
    else if (action.type == "KILL_ENEMY") {
        auto enemyId = action.getPayload<int>();
        auto& enemies = newState.enemies;
        enemies.erase(
            std::remove_if(enemies.begin(), enemies.end(),
                [enemyId](const EnemyData& e) { return e.id == enemyId; }),
            enemies.end()
        );
        newState.score += 100;
    }
    else if (action.type == "UPDATE_TIME") {
        auto delta = action.getPayload<float>();
        newState.time = state.time + delta;
    }

    return newState;
}

} // namespace Reactive
