#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System/Time.hpp>
#include <string>

using namespace sf;
using namespace std;


// Base Entity class - like React.Component but simplified for games
class Entity {
public:
    Entity(const string& name = "Entity")
        : entityName(name), active(true) {}

    virtual ~Entity() = default;

    // Lifecycle hooks (React-style! - all use on* prefix for consistency)
    virtual void onCreate() {}                  // Like componentDidMount
    virtual void onInput(Event& event) {}       // Handle input events (like onClick, onKeyPress)
    virtual void onUpdate(Time dt) {}           // Game logic every frame
    virtual void onDraw(RenderWindow& window) {} // Direct rendering
    virtual void onDestroy() {}                 // Like componentWillUnmount

    // Entity management
    string getName() const { return entityName; }
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
    string entityName;
    bool active;
};
