#pragma once

#include "../core/Component.h"
#include "../core/RenderNode.h"

namespace Reactive {

// HUD component for score and game info
class HUDComponent : public Component {
public:
    HUDComponent(int score, bool gameOver)
        : Component("HUD"),
          score(score),
          gameOver(gameOver) {}

    std::shared_ptr<RenderNode> render() override {
        auto container = Container();

        // Score display
        Props scoreProps;
        scoreProps["content"] = "Score: " + std::to_string(score);
        scoreProps["fontSize"] = 24;
        scoreProps["color"] = sf::Color::Yellow;
        scoreProps["position"] = sf::Vector2f(10, 10);

        auto scoreText = Text(scoreProps);
        scoreText->key = "score";

        container->children.push_back(scoreText);

        // Game over text
        if (gameOver) {
            Props gameOverProps;
            gameOverProps["content"] = "GAME OVER";
            gameOverProps["fontSize"] = 48;
            gameOverProps["color"] = sf::Color::Red;
            gameOverProps["position"] = sf::Vector2f(300, 250);

            auto gameOverText = Text(gameOverProps);
            gameOverText->key = "gameover";

            container->children.push_back(gameOverText);
        }

        return container;
    }

private:
    int score;
    bool gameOver;
};

} // namespace Reactive
