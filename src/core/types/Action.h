#pragma once

#include <string>
#include <any>


// Action (like Redux actions)
struct Action {
    std::string type;
    std::any payload;

    template<typename T>
    T getPayload() const {
        return std::any_cast<T>(payload);
    }
};
