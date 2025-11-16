#pragma once
#include "AGameObject.h"

/// <summary>
/// Background object that scrolls vertically to create a moving parallax effect
/// Uses texture repeating to create an infinite scroll
/// </summary>
class BGObject : public AGameObject
{
public: 
    BGObject(String name);
    void initialize();
    void processInput(sf::Event event);
    void update(sf::Time deltaTime);
    
private:
    const float SPEED_MULTIPLIER = 100.0f;  // Pixels per second
};
