#pragma once

#include "../core/Component.h"
#include "../core/RenderNode.h"
#include "GameState.h"
#include "BackgroundComponent.h"
#include "PlayerComponent.h"
#include "EnemyComponent.h"
#include "HUDComponent.h"
#include <memory>
#include <algorithm>

namespace ReactSFML {

// Main game component that composes all other components
class GameComponent : public Component {
public:
    GameComponent(Store<GameState>* store)
        : Component("Game"),
          store(store) {}

    void onMount() override {
        // Subscribe to store changes
        store->subscribe([this](const GameState& state) {
            this->forceUpdate();  // Re-render when state changes
        });

        // Create child components
        background = std::make_shared<BackgroundComponent>();
        player = std::make_shared<PlayerComponent>(
            store->getState().playerPosition,
            store->getState().playerHealth
        );

        background->mount();
        player->mount();
    }

    void onUpdate(sf::Time deltaTime) override {
        // Update player position from store
        const auto& state = store->getState();
        player->updatePosition(state.playerPosition);
        player->updateHealth(state.playerHealth);

        // Update or create enemy components
        syncEnemies(state.enemies);
    }

    std::shared_ptr<RenderNode> render() override {
        const auto& state = store->getState();
        auto container = Container();

        // Background layer
        container->children.push_back(background->render());

        // Game entities layer
        container->children.push_back(player->render());

        // Enemies
        for (auto& enemy : enemies) {
            container->children.push_back(enemy->render());
        }

        // HUD layer
        auto hud = std::make_shared<HUDComponent>(state.score, state.gameOver);
        container->children.push_back(hud->render());

        return container;
    }

private:
    void syncEnemies(const std::vector<EnemyData>& enemyDataList) {
        // Simple sync: recreate enemy components based on state
        // (In real implementation, would use key-based reconciliation)

        // Remove enemies not in state
        enemies.erase(
            std::remove_if(enemies.begin(), enemies.end(),
                [&enemyDataList](const std::shared_ptr<EnemyComponent>& enemy) {
                    return std::none_of(enemyDataList.begin(), enemyDataList.end(),
                        [&enemy](const EnemyData& data) {
                            return data.id == enemy->getId();
                        });
                }),
            enemies.end()
        );

        // Add new enemies from state
        for (const auto& data : enemyDataList) {
            bool exists = std::any_of(enemies.begin(), enemies.end(),
                [&data](const std::shared_ptr<EnemyComponent>& enemy) {
                    return enemy->getId() == data.id;
                });

            if (!exists) {
                auto enemy = std::make_shared<EnemyComponent>(data);
                enemy->mount();
                enemies.push_back(enemy);
            } else {
                // Update existing enemy position
                for (auto& enemy : enemies) {
                    if (enemy->getId() == data.id) {
                        enemy->updatePosition(data.position);
                    }
                }
            }
        }
    }

    Store<GameState>* store;
    std::shared_ptr<BackgroundComponent> background;
    std::shared_ptr<PlayerComponent> player;
    std::vector<std::shared_ptr<EnemyComponent>> enemies;
};

} // namespace ReactSFML
