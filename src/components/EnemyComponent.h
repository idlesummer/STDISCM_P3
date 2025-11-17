#pragma once

#include "../core/Component.h"
#include "../core/RenderNode.h"
#include "GameState.h"

namespace ReactSFML {

// Enemy component
class EnemyComponent : public Component {
public:
    EnemyComponent(const EnemyData& data)
        : Component("Enemy"),
          enemyData(data) {}

    void onMount() override {
        position = useState(enemyData.position);
        health = useState(sf::Vector2f(enemyData.health, 0));

        // Animate enemy movement
        useEffect([this]() {
            // This would be called each frame in a real implementation
            return []() {};  // No cleanup
        }, {});
    }

    std::shared_ptr<RenderNode> render() override {
        auto container = Container();

        // Enemy body (red circle)
        Props enemyProps;
        enemyProps["radius"] = 15.0f;
        enemyProps["color"] = sf::Color::Red;
        enemyProps["position"] = position.get();

        auto enemyCircle = Circle(enemyProps);
        enemyCircle->key = "enemy_" + std::to_string(enemyData.id);

        container->children.push_back(enemyCircle);

        return container;
    }

    void updatePosition(sf::Vector2f newPos) {
        position.set(newPos);
    }

    int getId() const { return enemyData.id; }

private:
    EnemyData enemyData;
    StateRef<sf::Vector2f> position;
    StateRef<sf::Vector2f> health;
};

} // namespace ReactSFML
