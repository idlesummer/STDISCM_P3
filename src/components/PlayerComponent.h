#pragma once

#include "../core/Component.h"
#include "../core/RenderNode.h"
#include "GameState.h"
#include <iostream>

namespace ReactSFML {

// Player component
class PlayerComponent : public Component {
public:
    PlayerComponent(sf::Vector2f position, int health)
        : Component("Player"),
          initialPosition(position),
          initialHealth(health) {}

    void onMount() override {
        // Initialize state when component mounts
        position = useState(initialPosition);
        health = useState(initialHealth);

        // Effect to log health changes
        useEffect([this]() {
            std::cout << "Player health: " << health.get() << std::endl;
            return []() {};  // No cleanup
        }, {});
    }

    std::shared_ptr<RenderNode> render() override {
        auto container = Container();

        // Player body (circle)
        Props playerProps;
        playerProps["radius"] = 20.0f;
        playerProps["color"] = sf::Color::Green;
        playerProps["position"] = position.get();

        auto playerCircle = Circle(playerProps);
        playerCircle->key = "player_body";

        // Health indicator (text above player)
        Props healthTextProps;
        healthTextProps["content"] = "HP: " + std::to_string(health.get());
        healthTextProps["fontSize"] = 16;
        healthTextProps["color"] = sf::Color::White;
        healthTextProps["position"] = position.get() + sf::Vector2f(-20, -40);

        auto healthText = Text(healthTextProps);
        healthText->key = "player_health";

        container->children.push_back(playerCircle);
        container->children.push_back(healthText);

        return container;
    }

    // External update from game loop
    void updatePosition(sf::Vector2f newPos) {
        position.set(newPos);
    }

    void updateHealth(int newHealth) {
        health.set(newHealth);
    }

private:
    sf::Vector2f initialPosition;
    int initialHealth;
    StateRef<sf::Vector2f> position;
    StateRef<int> health;
};

} // namespace ReactSFML
