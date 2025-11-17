#pragma once

#include <functional>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <string>


class Component; // Forward declaration

// State reference that components can use
template<typename T>
class StateRef {
public:
    // Default constructor (for member variable declaration)
    StateRef() : valuePtr(nullptr), setter(nullptr) {}

    StateRef(T* valuePtr, std::function<void(T)> setter)
        : valuePtr(valuePtr), setter(setter) {}

    T get() const {
        return !this->valuePtr ? T() : *this->valuePtr;
    }

    void set(T newValue) {
        if (!this->setter)
            return;
        this->setter(newValue);
    }

    // Convenient operators
    operator T() const { return this->get(); }
    StateRef& operator=(const T& value) {
        this->set(value);
        return *this;
    }

private:
    T* valuePtr;
    std::function<void(T)> setter;
};

// Manages state for all components (like React's hooks internals)
class StateManager {
public:
    static StateManager& getInstance() {
        static auto instance = StateManager();
        return instance;
    }

    template<typename T>
    StateRef<T> registerState(Component* component, T initialValue) {
        auto key = this->getStateKey(component);

        // Store state value
        if (this->stateValues.find(key) == this->stateValues.end())
            this->stateValues[key] = std::make_shared<T>(initialValue);

        auto valuePtr = std::static_pointer_cast<T>(this->stateValues[key]);

        // Create setter that triggers re-render
        auto setter = [this, component, valuePtr](T newValue) {
            *valuePtr = newValue;
            this->markForRerender(component);
        };

        return StateRef<T>(valuePtr.get(), setter);
    }

    void markForRerender(Component* component) {
        this->componentsToRerender.insert(component);
    }

    bool needsRerender(Component* component) const {
        return this->componentsToRerender.find(component) != this->componentsToRerender.end();
    }

    void clearRerenderFlag(Component* component) {
        this->componentsToRerender.erase(component);
    }

    void reset() {
        this->stateValues.clear();
        this->componentsToRerender.clear();
        this->stateCounter = 0;
    }

private:
    StateManager() : stateCounter(0) {}

    std::string getStateKey(Component* component) {
        return std::to_string(reinterpret_cast<uintptr_t>(component)) + "_" + std::to_string(this->stateCounter++);
    }

    std::unordered_map<std::string, std::shared_ptr<void>> stateValues;
    std::unordered_set<Component*> componentsToRerender;
    size_t stateCounter;
};
