#pragma once


// Base state class - derive your game state from this
class State {
public:
    virtual ~State() = default;
    virtual State* clone() const = 0;  // For time-travel debugging
};
