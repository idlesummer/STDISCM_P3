#pragma once

#include "../types/Action.h"
#include <functional>
#include <iostream>


// Middleware function type (for logging, async, etc.)
using Middleware = std::function<void(const Action&)>;

// Common middleware implementations

// Logger middleware
inline Middleware createLoggerMiddleware() {
    return [](const Action& action) {
        std::cout << "[ACTION] " << action.type << std::endl;
    };
}

// Forward declaration for thunk middleware
template<typename TState>
class Store;

// Thunk middleware (for async actions)
template<typename TState>
using ThunkAction = std::function<void(Store<TState>&)>;
