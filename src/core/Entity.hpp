#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System/Time.hpp>
#include <string>

using namespace sf;
using namespace std;


// Base Entity class - like React.Component but simplified for games
class Entity {
protected:
    string entityName;
    bool active;
    Vector2f position;

public:
    Entity(const string& name = "Entity", Vector2f initialPosition = Vector2f(0, 0))
        : entityName(name),
            active(true),
            position(initialPosition) {}

    virtual ~Entity() = default;

    // Lifecycle hooks (React-style! - all use on* prefix for consistency)
    virtual void onCreate() {}                  // Like componentDidMount
    virtual void onInput(Event& event) {}       // Handle input events (like onClick, onKeyPress)
    virtual void onUpdate(Time dt) {}           // Game logic every frame
    virtual void onDraw(RenderWindow& window) {} // Direct rendering
    virtual void onDestroy() {}                 // Like componentWillUnmount

    // Entity management
    auto getName() const { return this->entityName; }
    auto isActive() const { return this->active; }
    void setActive(bool isActive) { this->active = isActive; }

    // Position management
    auto getPosition() const { return this->position; }
    void setPosition(Vector2f newPosition) { this->position = newPosition; }
};
