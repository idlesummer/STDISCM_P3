#pragma once

#include "../types/Props.h"
#include "../hooks/StateHook.h"
#include "../hooks/EffectHook.h"
#include <SFML/System/Time.hpp>
#include <memory>
#include <string>
#include <functional>
#include <any>
#include <vector>


// Base component class (like React.Component)
class Component {
public:
    Component(const std::string& name = "Component")
        : componentName(name), mounted(false) {}

    virtual ~Component() {
        if (mounted)
            EffectManager::getInstance().cleanup(this);
    }

    // Main render method - returns virtual DOM tree
    virtual std::shared_ptr<RenderNode> render() = 0;

    // Lifecycle methods
    virtual void onMount() {}
    virtual void onUnmount() {}
    virtual void onUpdate(sf::Time deltaTime) {}

    // Component identity
    std::string getName() const { return this->componentName; }
    bool isMounted() const { return this->mounted; }

    void mount() {
        if (this->mounted)
            return;
        this->mounted = true;
        this->onMount();
    }

    void unmount() {
        if (!this->mounted)
            return;
        this->onUnmount();
        EffectManager::getInstance().cleanup(this);
        this->mounted = false;
    }

protected:
    // Hook: useState - Returns a state reference
    template<typename T>
    StateRef<T> useState(T initialValue) {
        return StateManager::getInstance().registerState<T>(this, initialValue);
    }

    // Hook: useEffect - Run side effects when dependencies change
    void useEffect(std::function<void()> effect, std::vector<std::any> dependencies = {}) {
        EffectManager::getInstance().registerEffect(this, effect, dependencies);
    }

    // Hook: useEffect with cleanup
    void useEffect(std::function<std::function<void()>()> effect, std::vector<std::any> dependencies = {}) {
        EffectManager::getInstance().registerEffectWithCleanup(this, effect, dependencies);
    }

    // Trigger re-render
    void forceUpdate() {
        StateManager::getInstance().markForRerender(this);
    }

private:
    std::string componentName;
    bool mounted;
};

// Higher-order component helpers
using ComponentFactory = std::function<std::shared_ptr<Component>()>;
