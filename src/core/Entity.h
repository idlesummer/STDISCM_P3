#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System/Time.hpp>
#include <string>


// Base Entity class - like React.Component but simplified for games
class Entity {
public:
    Entity(const std::string& name = "Entity")
        : entityName(name), active(true) {}

    virtual ~Entity() = default;

    // Lifecycle hooks (React-style!)
    virtual void onCreate() {}              // Like componentDidMount
    virtual void update(sf::Time dt) {}     // Game logic every frame
    virtual void draw(sf::RenderWindow& window) {}  // Direct rendering
    virtual void onDestroy() {}             // Like componentWillUnmount

    // Entity management
    std::string getName() const { return entityName; }
    bool isActive() const { return active; }
    void setActive(bool isActive) { active = isActive; }

    // Helper for state management (optional)
    template<typename T>
    class State {
        T value;
    public:
        State(T v) : value(v) {}
        T get() const { return value; }
        void set(T v) { value = v; }

        // Convenience operators
        operator T() const { return value; }
        State& operator=(T v) { value = v; return *this; }
    };

protected:
    std::string entityName;
    bool active;
};
