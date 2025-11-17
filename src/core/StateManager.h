#pragma once

#include <functional>
#include <vector>
#include <memory>
#include <any>
#include <unordered_map>
#include <unordered_set>

namespace ReactSFML {

class Component; // Forward declaration

// State reference that components can use
template<typename T>
class StateRef {
public:
    StateRef(T* valuePtr, std::function<void(T)> setter)
        : valuePtr(valuePtr), setter(setter) {}

    T get() const { return *valuePtr; }
    void set(T newValue) { setter(newValue); }

    // Convenient operators
    operator T() const { return get(); }
    StateRef& operator=(const T& value) { set(value); return *this; }

private:
    T* valuePtr;
    std::function<void(T)> setter;
};

// Manages state for all components (like React's hooks internals)
class StateManager {
public:
    static StateManager& getInstance() {
        static StateManager instance;
        return instance;
    }

    template<typename T>
    StateRef<T> registerState(Component* component, T initialValue) {
        auto key = getStateKey(component);

        // Store state value
        if (stateValues.find(key) == stateValues.end()) {
            stateValues[key] = std::make_shared<T>(initialValue);
        }

        auto valuePtr = std::static_pointer_cast<T>(stateValues[key]);

        // Create setter that triggers re-render
        auto setter = [this, component, valuePtr](T newValue) {
            *valuePtr = newValue;
            markForRerender(component);
        };

        return StateRef<T>(valuePtr.get(), setter);
    }

    void markForRerender(Component* component) {
        componentsToRerender.insert(component);
    }

    bool needsRerender(Component* component) const {
        return componentsToRerender.find(component) != componentsToRerender.end();
    }

    void clearRerenderFlag(Component* component) {
        componentsToRerender.erase(component);
    }

    void reset() {
        stateValues.clear();
        componentsToRerender.clear();
        stateCounter = 0;
    }

private:
    StateManager() : stateCounter(0) {}

    std::string getStateKey(Component* component) {
        return std::to_string(reinterpret_cast<uintptr_t>(component)) + "_" + std::to_string(stateCounter++);
    }

    std::unordered_map<std::string, std::shared_ptr<void>> stateValues;
    std::unordered_set<Component*> componentsToRerender;
    size_t stateCounter;
};

} // namespace ReactSFML
