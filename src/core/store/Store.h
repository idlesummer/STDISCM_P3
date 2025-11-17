#pragma once

#include "../types/Action.h"
#include "Middleware.h"
#include <functional>
#include <vector>


// Reducer function type
template<typename TState>
using Reducer = std::function<TState(const TState&, const Action&)>;

// Store (like Redux store)
template<typename TState>
class Store {
public:
    Store(TState initialState, Reducer<TState> reducer)
        : state(initialState), reducer(reducer) {}

    // Get current state (read-only)
    const TState& getState() const {
        return this->state;
    }

    // Dispatch an action
    void dispatch(const Action& action) {
        // Run middleware before reducer
        for (auto& middleware : this->middlewares)
            middleware(action);

        // Run reducer
        auto newState = this->reducer(this->state, action);

        // Check if state actually changed
        auto changed = true;  // Simplified - would need deep comparison
        if (!changed)
            return;

        this->state = newState;

        // Notify all subscribers
        for (auto& subscriber : this->subscribers)
            subscriber(this->state);
    }

    // Subscribe to state changes
    void subscribe(std::function<void(const TState&)> callback) {
        this->subscribers.push_back(callback);
    }

    // Add middleware
    void addMiddleware(Middleware middleware) {
        this->middlewares.push_back(middleware);
    }

    // Clear all subscribers (useful for cleanup)
    void clearSubscribers() {
        this->subscribers.clear();
    }

private:
    TState state;
    Reducer<TState> reducer;
    std::vector<std::function<void(const TState&)>> subscribers;
    std::vector<Middleware> middlewares;
};
