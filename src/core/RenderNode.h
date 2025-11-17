#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <memory>
#include <variant>
#include <unordered_map>

namespace Reactive {

// Type-safe props using variant
using PropValue = std::variant<
    int, float, std::string, bool,
    sf::Vector2f, sf::Color,
    sf::Texture*
>;

using Props = std::unordered_map<std::string, PropValue>;

// Render node types (like React element types)
enum class NodeType {
    Container,    // Group of children (like <div>)
    Sprite,       // sf::Sprite wrapper
    Text,         // sf::Text wrapper
    Rectangle,    // sf::RectangleShape wrapper
    Circle        // sf::CircleShape wrapper
};

// Virtual DOM node (like React elements)
class RenderNode {
public:
    NodeType type;
    Props props;
    std::vector<std::shared_ptr<RenderNode>> children;
    std::string key;  // For reconciliation (like React keys)

    RenderNode(NodeType type, Props props = {})
        : type(type), props(props), key("") {}

    RenderNode(NodeType type, Props props, std::vector<std::shared_ptr<RenderNode>> children)
        : type(type), props(props), children(children), key("") {}

    // Helper to get prop with default value
    template<typename T>
    T getProp(const std::string& name, T defaultValue) const {
        auto it = this->props.find(name);
        if (it == this->props.end())
            return defaultValue;

        if (auto* val = std::get_if<T>(&it->second))
            return *val;

        return defaultValue;
    }

    // Builder pattern for cleaner syntax
    RenderNode& withKey(const std::string& k) {
        this->key = k;
        return *this;
    }

    RenderNode& withChild(std::shared_ptr<RenderNode> child) {
        this->children.push_back(child);
        return *this;
    }
};

// Factory functions for cleaner syntax
inline std::shared_ptr<RenderNode> Container(Props props = {}) {
    return std::make_shared<RenderNode>(NodeType::Container, props);
}

inline std::shared_ptr<RenderNode> Sprite(Props props) {
    return std::make_shared<RenderNode>(NodeType::Sprite, props);
}

inline std::shared_ptr<RenderNode> Text(Props props) {
    return std::make_shared<RenderNode>(NodeType::Text, props);
}

inline std::shared_ptr<RenderNode> Rectangle(Props props) {
    return std::make_shared<RenderNode>(NodeType::Rectangle, props);
}

inline std::shared_ptr<RenderNode> Circle(Props props) {
    return std::make_shared<RenderNode>(NodeType::Circle, props);
}

} // namespace Reactive
