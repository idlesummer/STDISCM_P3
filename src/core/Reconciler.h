#pragma once

#include "RenderNode.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <unordered_map>
#include <string>

namespace ReactSFML {

// Represents an actual SFML drawable object
struct SFMLInstance {
    std::shared_ptr<sf::Drawable> drawable;
    std::shared_ptr<sf::Transformable> transformable;  // For position, rotation, scale
    NodeType type;
    std::vector<std::shared_ptr<SFMLInstance>> children;

    // Font for text nodes (shared resource)
    inline static sf::Font defaultFont;
};

// Change types for reconciliation
enum class ChangeType {
    Create,
    Update,
    Delete,
    Reorder
};

struct Change {
    ChangeType type;
    std::shared_ptr<RenderNode> node;
    std::shared_ptr<SFMLInstance> instance;
};

// Reconciles virtual DOM with actual SFML objects
class Reconciler {
public:
    Reconciler(sf::RenderWindow* window)
        : window(window), rootInstance(nullptr) {
        // Load default font
        if (!SFMLInstance::defaultFont.loadFromFile("Media/arial.ttf")) {
            // Fallback - try system font
            SFMLInstance::defaultFont.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");
        }
    }

    // Main reconciliation method - diffs and updates
    void reconcile(std::shared_ptr<RenderNode> newTree) {
        if (!previousTree) {
            // First render - create everything
            rootInstance = createInstance(newTree);
        } else {
            // Subsequent render - diff and patch
            auto changes = diff(previousTree, newTree, rootInstance);
            applyChanges(changes);
        }

        previousTree = newTree;
    }

    // Render all SFML objects to window
    void render() {
        if (rootInstance) {
            window->clear(sf::Color::Black);
            renderInstance(rootInstance);
            window->display();
        }
    }

    void reset() {
        previousTree = nullptr;
        rootInstance = nullptr;
        instanceCache.clear();
    }

private:
    // Create SFML instance from render node
    std::shared_ptr<SFMLInstance> createInstance(std::shared_ptr<RenderNode> node) {
        auto instance = std::make_shared<SFMLInstance>();
        instance->type = node->type;

        switch (node->type) {
            case NodeType::Sprite: {
                auto sprite = std::make_shared<sf::Sprite>();

                // Set texture if provided
                if (auto* texture = std::get_if<sf::Texture*>(&node->props["texture"])) {
                    sprite->setTexture(**texture);
                }

                instance->drawable = sprite;
                instance->transformable = sprite;
                break;
            }

            case NodeType::Text: {
                auto text = std::make_shared<sf::Text>();
                text->setFont(SFMLInstance::defaultFont);
                text->setString(node->getProp<std::string>("content", ""));
                text->setCharacterSize(node->getProp<int>("fontSize", 24));
                text->setFillColor(node->getProp<sf::Color>("color", sf::Color::White));

                instance->drawable = text;
                instance->transformable = text;
                break;
            }

            case NodeType::Rectangle: {
                auto rect = std::make_shared<sf::RectangleShape>();
                auto size = node->getProp<sf::Vector2f>("size", sf::Vector2f(100, 100));
                rect->setSize(size);
                rect->setFillColor(node->getProp<sf::Color>("color", sf::Color::White));

                instance->drawable = rect;
                instance->transformable = rect;
                break;
            }

            case NodeType::Circle: {
                auto circle = std::make_shared<sf::CircleShape>();
                circle->setRadius(node->getProp<float>("radius", 50.0f));
                circle->setFillColor(node->getProp<sf::Color>("color", sf::Color::White));

                instance->drawable = circle;
                instance->transformable = circle;
                break;
            }

            case NodeType::Container:
                // Container has no drawable, just children
                break;
        }

        // Apply common transformations
        if (instance->transformable) {
            instance->transformable->setPosition(node->getProp<sf::Vector2f>("position", sf::Vector2f(0, 0)));
            instance->transformable->setScale(node->getProp<sf::Vector2f>("scale", sf::Vector2f(1, 1)));
            instance->transformable->setRotation(node->getProp<float>("rotation", 0.0f));
        }

        // Create children recursively
        for (auto& child : node->children) {
            instance->children.push_back(createInstance(child));
        }

        // Cache by key if provided
        if (!node->key.empty()) {
            instanceCache[node->key] = instance;
        }

        return instance;
    }

    // Diff two trees and generate changes
    std::vector<Change> diff(std::shared_ptr<RenderNode> oldNode,
                            std::shared_ptr<RenderNode> newNode,
                            std::shared_ptr<SFMLInstance> instance) {
        std::vector<Change> changes;

        // Node types differ - recreate
        if (oldNode->type != newNode->type) {
            changes.push_back({ChangeType::Delete, oldNode, instance});
            changes.push_back({ChangeType::Create, newNode, nullptr});
            return changes;
        }

        // Same type - check for prop changes
        if (propsChanged(oldNode->props, newNode->props)) {
            changes.push_back({ChangeType::Update, newNode, instance});
        }

        // Diff children (simplified - no key-based reordering for now)
        size_t minChildren = std::min(oldNode->children.size(), newNode->children.size());

        for (size_t i = 0; i < minChildren; i++) {
            auto childChanges = diff(oldNode->children[i], newNode->children[i], instance->children[i]);
            changes.insert(changes.end(), childChanges.begin(), childChanges.end());
        }

        // New children added
        for (size_t i = minChildren; i < newNode->children.size(); i++) {
            changes.push_back({ChangeType::Create, newNode->children[i], nullptr});
        }

        // Old children removed
        for (size_t i = minChildren; i < oldNode->children.size(); i++) {
            changes.push_back({ChangeType::Delete, oldNode->children[i], instance->children[i]});
        }

        return changes;
    }

    // Check if props changed
    bool propsChanged(const Props& oldProps, const Props& newProps) {
        if (oldProps.size() != newProps.size())
            return true;

        for (const auto& [key, value] : newProps) {
            auto it = oldProps.find(key);
            if (it == oldProps.end() || it->second.index() != value.index())
                return true;
            // Simplified - would need deep comparison per type
        }

        return false;
    }

    // Apply changes to SFML instances
    void applyChanges(const std::vector<Change>& changes) {
        for (const auto& change : changes) {
            switch (change.type) {
                case ChangeType::Create:
                    // Create new instance and add to parent
                    createInstance(change.node);
                    break;

                case ChangeType::Update:
                    // Update existing instance properties
                    updateInstance(change.instance, change.node);
                    break;

                case ChangeType::Delete:
                    // Remove from parent (handled by parent update)
                    break;

                case ChangeType::Reorder:
                    // Reorder children (for key-based reconciliation)
                    break;
            }
        }
    }

    // Update an existing SFML instance
    void updateInstance(std::shared_ptr<SFMLInstance> instance, std::shared_ptr<RenderNode> node) {
        // Update transformations
        if (instance->transformable) {
            instance->transformable->setPosition(node->getProp<sf::Vector2f>("position", sf::Vector2f(0, 0)));
            instance->transformable->setScale(node->getProp<sf::Vector2f>("scale", sf::Vector2f(1, 1)));
            instance->transformable->setRotation(node->getProp<float>("rotation", 0.0f));
        }

        // Type-specific updates
        switch (instance->type) {
            case NodeType::Text: {
                auto text = std::static_pointer_cast<sf::Text>(instance->drawable);
                text->setString(node->getProp<std::string>("content", ""));
                text->setCharacterSize(node->getProp<int>("fontSize", 24));
                text->setFillColor(node->getProp<sf::Color>("color", sf::Color::White));
                break;
            }

            case NodeType::Rectangle: {
                auto rect = std::static_pointer_cast<sf::RectangleShape>(instance->drawable);
                rect->setSize(node->getProp<sf::Vector2f>("size", sf::Vector2f(100, 100)));
                rect->setFillColor(node->getProp<sf::Color>("color", sf::Color::White));
                break;
            }

            case NodeType::Circle: {
                auto circle = std::static_pointer_cast<sf::CircleShape>(instance->drawable);
                circle->setRadius(node->getProp<float>("radius", 50.0f));
                circle->setFillColor(node->getProp<sf::Color>("color", sf::Color::White));
                break;
            }

            default:
                break;
        }
    }

    // Recursively render instance and children
    void renderInstance(std::shared_ptr<SFMLInstance> instance) {
        if (instance->drawable) {
            window->draw(*instance->drawable);
        }

        for (auto& child : instance->children) {
            renderInstance(child);
        }
    }

    sf::RenderWindow* window;
    std::shared_ptr<RenderNode> previousTree;
    std::shared_ptr<SFMLInstance> rootInstance;
    std::unordered_map<std::string, std::shared_ptr<SFMLInstance>> instanceCache;
};

} // namespace ReactSFML
