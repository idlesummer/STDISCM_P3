#pragma once

#include "../types/Props.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>


// Represents an actual SFML drawable object
struct SFMLInstance {
    std::shared_ptr<sf::Drawable> drawable;
    std::shared_ptr<sf::Transformable> transformable;  // For position, rotation, scale
    NodeType type;
    std::vector<std::shared_ptr<SFMLInstance>> children;

    // Font for text nodes (shared resource)
    inline static sf::Font defaultFont;
};
