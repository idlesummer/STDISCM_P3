#pragma once

#include "../core/Component.h"
#include "../core/RenderNode.h"

namespace ReactSFML {

// Simple background component
class BackgroundComponent : public Component {
public:
    BackgroundComponent() : Component("Background") {}

    std::shared_ptr<RenderNode> render() override {
        // Create a simple gradient-like background using rectangles
        auto container = Container();

        // Dark blue background
        Props bgProps;
        bgProps["size"] = sf::Vector2f(800, 600);
        bgProps["color"] = sf::Color(20, 20, 40);
        bgProps["position"] = sf::Vector2f(0, 0);

        auto background = Rectangle(bgProps);
        container->children.push_back(background);

        return container;
    }
};

} // namespace ReactSFML
