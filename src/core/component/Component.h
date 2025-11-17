#pragma once

#include "../types/Props.h"
#include <SFML/System/Time.hpp>
#include <functional>
#include <memory>
#include <string>


// Base component class (like React.Component - class-based)
class Component {
public:
    Component(const std::string& name = "Component")
        : componentName(name), mounted(false) {}

    virtual ~Component() = default;

    // Main render method - returns virtual DOM tree
    virtual std::shared_ptr<RenderNode> render() = 0;

    // Lifecycle methods (like class-based React)
    virtual void componentDidMount() {}
    virtual void componentWillUnmount() {}
    virtual void componentDidUpdate() {}

    // Update hook (for game logic)
    virtual void onUpdate(sf::Time deltaTime) {}

    // Component identity
    std::string getName() const { return this->componentName; }
    bool isMounted() const { return this->mounted; }

    void mount() {
        if (this->mounted)
            return;
        this->mounted = true;
        this->componentDidMount();
    }

    void unmount() {
        if (!this->mounted)
            return;
        this->componentWillUnmount();
        this->mounted = false;
    }

protected:
    // Trigger re-render (like this.setState() in React)
    void setState() {
        // Component always re-renders on next frame
        // No complex state management needed
    }

private:
    std::string componentName;
    bool mounted;
};

// Higher-order component helpers
using ComponentFactory = std::function<std::shared_ptr<Component>()>;
